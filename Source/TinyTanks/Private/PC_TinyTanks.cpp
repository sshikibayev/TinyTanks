


#include "PC_TinyTanks.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NavigationSystem.h"
#include "TinyTankProjectile.h"
#include "TinyTankCharacter.h"
#include "Kismet/GamePlayStatics.h"
#include "UObject/ConstructorHelpers.h"
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

void APC_TinyTanks::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APC_TinyTanks::BeginPlay()
{
    Super::BeginPlay();

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    SetInputMode(InputMode);
}

void APC_TinyTanks::SetupInputComponent()
{
    Super::SetupInputComponent();
    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IMC_TinyTanks);
    BindInputActions();
}

void APC_TinyTanks::OnInputStarted()
{
    StopMovement();

    Server_StopMovement();
}

void APC_TinyTanks::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }

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

void APC_TinyTanks::OnTouchTriggered()
{
    OnSetDestinationTriggered();
}

void APC_TinyTanks::OnTouchReleased()
{
    OnSetDestinationReleased();
}

void APC_TinyTanks::LaunchFire()
{
    if (!bFiringWeapon)
    {
        bFiringWeapon = true;
        GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &ThisClass::StopFire, FireRate, false);
        Server_HandleFire();
    }
}

void APC_TinyTanks::StopFire()
{
    bFiringWeapon = false;
}

void APC_TinyTanks::OnFirePressed()
{
    LaunchFire();
}

void APC_TinyTanks::Server_HandleFire_Implementation()
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
        Projectile ? Projectile->SetOwner(this) : nullptr;
    }
}

void APC_TinyTanks::Server_NavigationMove_Implementation(const FVector& TargetDestionation)
{
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetPawn()->GetController(), TargetDestionation);
}

void APC_TinyTanks::Server_StopMovement_Implementation()
{
    StopMovement();
}

void APC_TinyTanks::PrepareInputSubsystem()
{
    TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PlayerController)
        return;

    TObjectPtr<ULocalPlayer> LocalPlayer = PlayerController->GetLocalPlayer();
    if (LocalPlayer)
    {
        InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
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
    if (!PlayerEnhancedInputComponent)
        return;

    PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Started, this, &ThisClass::OnInputStarted);
    PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Triggered, this, &ThisClass::OnSetDestinationTriggered);
    PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
    PlayerEnhancedInputComponent->BindAction(IA_SetDestinationByClick.Get(), ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);

    PlayerEnhancedInputComponent->BindAction(IA_Fire.Get(), ETriggerEvent::Started, this, &ThisClass::OnFirePressed);
}


void APC_TinyTanks::OneTouchAction()
{
    if (FollowTime <= ShortPressThreshold)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
        Server_NavigationMove(CachedDestination);
    }
}