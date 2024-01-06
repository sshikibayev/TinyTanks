


#include "Character/PC_TinyTanks.h"

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
#include "Navigation/PathFollowingComponent.h"
#include "AI/PC_AIController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"


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

void APC_TinyTanks::SetupInputComponent()
{
    Super::SetupInputComponent();

    PrepareInputSubsystem();
    AddingMappingContext(InputSubsystem, IMC_TinyTanks);
    BindInputActions();
}

void APC_TinyTanks::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!HasAuthority() && bContinuesMovementHold)
    {
        ServerStartContinuesMovement(CachedDestination);
    }
}

void APC_TinyTanks::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    CharacterSpawned();
}

void APC_TinyTanks::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

void APC_TinyTanks::SetTinyTankCharacter(const TObjectPtr<ATinyTankCharacter> NewTinyTankCharacter)
{
    TinyTankCharacter = NewTinyTankCharacter;
    OnSpawnEventBind();
}

void APC_TinyTanks::CharacterSpawned()
{
    OnCharacterSpawn.Broadcast();
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
    if (HasAuthority() && TinyTankCharacter)
    {
        TinyTankCharacter->GetController()->StopMovement();
    }
    else
    {
        ServerStopMovement();
    }
}

void APC_TinyTanks::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }

    if (HasAuthority())
    {
        ContinuesMovement(CachedDestination);
    }
    else
    {
        bContinuesMovementHold = true;
    }
}

void APC_TinyTanks::ServerStartContinuesMovement_Implementation(const FVector& Destination)
{
    ContinuesMovement(Destination);
}

void APC_TinyTanks::ServerStopMovement_Implementation()
{
    TinyTankCharacter->GetController()->StopMovement();
}

void APC_TinyTanks::ContinuesMovement(const FVector& Destination)
{
    if (TinyTankCharacter)
    {
        FVector WorldDirection = (Destination - TinyTankCharacter->GetActorLocation()).GetSafeNormal();
        TinyTankCharacter->AddMovementInput(WorldDirection);
    }
}

void APC_TinyTanks::OnSetDestinationReleased()
{
    OneTouchAction();
    FollowTime = 0.f;
    bContinuesMovementHold = false;
}

void APC_TinyTanks::OneTouchAction()
{
    if (FollowTime <= ShortPressThreshold)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector::One(), true, true, ENCPoolMethod::None, true);

        ServerSmartMove(CachedDestination);
    }
}

void APC_TinyTanks::ServerSmartMove_Implementation(const FVector& Destination)
{
    SmartMovement(Destination);
}

void APC_TinyTanks::SmartMovement(const FVector& Destination)
{
    if (TinyTankCharacter)
    {
        TObjectPtr<APC_AIController> PC_AI{ Cast<APC_AIController>(TinyTankCharacter->GetController()) };
        if (PC_AI)
        {
            PC_AI->SmartMoveToLocation(Destination);
        }
    }
}

void APC_TinyTanks::OnSpawnEventBind()
{
    if (TinyTankCharacter)
    {
        TinyTankCharacter->OnSpawn.AddDynamic(this, &ThisClass::CharacterSpawned);
    }
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

void APC_TinyTanks::DrawDebugRayFromMouseClick()
{
    FVector WorldLocation;
    FVector WorldDirection;
    DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
    FVector TraceEnd = WorldLocation + WorldDirection * 10000.0f;
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);
    if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, TraceEnd, ECC_GameTraceChannel10, CollisionParams))
    {
        DrawDebugLine(GetWorld(), WorldLocation, HitResult.ImpactPoint, FColor::Green, false, -1, 0, 1);
        UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitResult.GetActor()->GetName());
    }
    else
    {
        DrawDebugLine(GetWorld(), WorldLocation, TraceEnd, FColor::Red, false, -1, 0, 1);
    }
}