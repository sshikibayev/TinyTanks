


#include "Character/PC_TinyTanks.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Projectile/TinyTankProjectile.h"
#include "Character/TinyTankCharacter.h"
#include "Widgets/Scoreboard/W_Scoreboard.h"
#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Kismet/GamePlayStatics.h"
#include "Net/UnrealNetwork.h"

APC_TinyTanks::APC_TinyTanks()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.f;
}

void APC_TinyTanks::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    ScoreboardInitialization();
}

void APC_TinyTanks::BeginPlay()
{
    Super::BeginPlay();

    SetupInputMode();
}

void APC_TinyTanks::ScoreboardInitialization()
{
    if (GetNetMode() != ENetMode::NM_DedicatedServer)
    {
        if (WBP_Scoreboard = CreateWidget<UW_Scoreboard>(GetWorld(), ScoreboardClass))
        {
            WBP_Scoreboard->AddToViewport();
        }
    }
}

void APC_TinyTanks::AddToScoreboard(const TObjectPtr<UW_PlayerData> Widget)
{
    if (WBP_Scoreboard)
    {
        WBP_Scoreboard->AddWidget(Widget);
    }
}

void APC_TinyTanks::SetupInputMode()
{
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    SetInputMode(InputMode);
}

void APC_TinyTanks::StopAllMovements()
{
    StopMovement();
    ServerStopMovement();
}

void APC_TinyTanks::ServerStopMovement_Implementation()
{
    StopMovement();
}

void APC_TinyTanks::SetupInputComponent()
{
    Super::SetupInputComponent();

    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IMC_TinyTanks);
    BindInputActions();
}

void APC_TinyTanks::PrepareInputSubsystem()
{
    TObjectPtr<APlayerController> PlayerController{ Cast<APlayerController>(GetWorld()->GetFirstPlayerController()) };
    if (PlayerController)
    {
        if (TObjectPtr<ULocalPlayer> LocalPlayer = PlayerController->GetLocalPlayer())
        {
            InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
        }
    }
}

void APC_TinyTanks::AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext)
{
    if (Subsystem && !MappingContext.IsNull())
    {
        Subsystem->ClearAllMappings();
        Subsystem->AddMappingContext(MappingContext.LoadSynchronous(), 0);
    }
}

void APC_TinyTanks::BindInputActions()
{
    TObjectPtr<UEnhancedInputComponent> PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    if (PlayerEnhancedInputComponent)
    {
        PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
        PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Triggered, this, &ThisClass::OnSetDestinationTriggered);
        PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
        PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);

        PlayerEnhancedInputComponent->BindAction(IA_Fire.Get(), ETriggerEvent::Started, this, &ThisClass::OnFirePressed);
    }
}

void APC_TinyTanks::OnInputStarted()
{
    StopMovement();
    ServerStopMovement();
}

void APC_TinyTanks::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }

    MakeContinuesMovement();
}

void APC_TinyTanks::MakeContinuesMovement()
{
    TObjectPtr<APawn> TinyTankPawn{ GetPawn() };
    if (TinyTankPawn)
    {
        FVector WorldDirection = (CachedDestination - TinyTankPawn->GetActorLocation()).GetSafeNormal();
        TinyTankPawn->AddMovementInput(WorldDirection, 1.0, false);
    }
}

void APC_TinyTanks::OnSetDestinationReleased()
{
    OneTouchAction();
    FollowTime = 0.f;
}

void APC_TinyTanks::OneTouchAction()
{
    if (FollowTime <= ShortPressThreshold)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector::One(), true, true, ENCPoolMethod::None, true);

        ServerNavigationMove(CachedDestination);
    }
}

void APC_TinyTanks::ServerNavigationMove_Implementation(const FVector& TargetDestination)
{
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetPawn()->GetController(), TargetDestination);
}

void APC_TinyTanks::OnFirePressed()
{
    if (!bFiringWeapon)
    {
        bFiringWeapon = true;
        GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &ThisClass::StopFire, FireRate, false);
        ServerHandleFire();
    }
}

void APC_TinyTanks::ServerHandleFire_Implementation()
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Instigator = GetInstigator();
    SpawnParameters.Owner = this;
    TObjectPtr<ATinyTankCharacter> TinyTankCharacter{ Cast<ATinyTankCharacter>(GetPawn()) };
    if (TinyTankCharacter)
    {
        TObjectPtr<ATinyTankProjectile> Projectile = GetWorld()->SpawnActor<ATinyTankProjectile>
            (
                TinyTankCharacter->GetProjectileClass(),
                TinyTankCharacter->GetProjectileSpawnPoint()->GetComponentLocation(),
                TinyTankCharacter->GetProjectileSpawnPoint()->GetComponentRotation(),
                SpawnParameters
            );
    }
}

void APC_TinyTanks::StopFire()
{
    bFiringWeapon = false;
}