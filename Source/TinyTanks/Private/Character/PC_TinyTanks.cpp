


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

}

void APC_TinyTanks::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

}

void APC_TinyTanks::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APC_TinyTanks::Destroyed()
{
    RemoveAllBondedEvents();

    Super::Destroyed();
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

    SpawnedCharacterInitialization();
}

void APC_TinyTanks::SpawnedCharacterInitialization()
{
    BindOnSpawnEvent();
    BindOnDeadEvent();
}

void APC_TinyTanks::BindOnSpawnEvent()
{
    if (TinyTankCharacter)
    {
        TinyTankCharacter->OnSpawn.AddDynamic(this, &ThisClass::CharacterSpawned);
    }
}

void APC_TinyTanks::BindOnDeadEvent()
{
    if (TinyTankCharacter)
    {
        TinyTankCharacter->OnDeath.AddDynamic(this, &ThisClass::CharacterDead);
    }
}

void APC_TinyTanks::CharacterSpawned()
{
    OnCharacterSpawn.Broadcast();
}

void APC_TinyTanks::CharacterDead()
{
    RemoveAllBondedEvents();

    OnCharacterDeath.Broadcast();
}

void APC_TinyTanks::RemoveAllBondedEvents()
{
    if (TinyTankCharacter)
    {
        TinyTankCharacter->OnSpawn.RemoveDynamic(this, &ThisClass::CharacterSpawned);
        TinyTankCharacter->OnDeath.RemoveDynamic(this, &ThisClass::CharacterDead);
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
}

void APC_TinyTanks::StopMovement()
{
    if (HasAuthority())
    {
        if (TinyTankCharacter)
        {
            TinyTankCharacter->StopMovement();
        }
    }
    else
    {
        ServerStopMovement();
    }
}

void APC_TinyTanks::ServerStopMovement_Implementation()
{
    StopMovement();
}

void APC_TinyTanks::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();

    if (!bMovementHolding)
    {
        bMovementHolding = true;
        GetWorld()->GetTimerManager().SetTimer(MovementHoldingTimer, this, &ThisClass::RefreshMovementHold, MovementHoldRate, false);

        CalculateDestination();
        SetDestinationForTheTinyTank(CachedDestination);
    }
}

void APC_TinyTanks::CalculateDestination()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }
}

void APC_TinyTanks::ServerSetDestination_Implementation(const FVector& Destination)
{
    SetDestinationForTheTinyTank(Destination);
}

void APC_TinyTanks::SetDestinationForTheTinyTank(const FVector& Destination)
{
    //Check is it Listen server or Client,
    //Dedicated server doesnt exist in this function,
    //Due to the function was called from input.
    if (HasAuthority())
    {
        if (TinyTankCharacter)
        {
            TinyTankCharacter->ReceiveDestinationForMovement(Destination);
        }
    }
    else
    {
        //We have to call RPC to set Destination from the DedicatedServer,
        //Because it's function called from the input.
        ServerSetDestination(CachedDestination);
    }
}

void APC_TinyTanks::OnSetDestinationReleased()
{
    StopMovement();
    OneTouchAction();
    FollowTime = 0.f;
}

void APC_TinyTanks::OneTouchAction()
{
    if (FollowTime <= ShortPressThreshold)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector::One(), true, true, ENCPoolMethod::None, true);

        if (HasAuthority())
        {
            SmartMovement(CachedDestination);
        }
        else
        {
            ServerSmartMove(CachedDestination);
        }
    }
}

void APC_TinyTanks::RefreshMovementHold()
{
    bMovementHolding = false;
}

void APC_TinyTanks::ServerSmartMove_Implementation(const FVector& Destination)
{
    SmartMovement(Destination);
}

void APC_TinyTanks::SmartMovement(const FVector& Destination)
{
    if (TinyTankCharacter)
    {
        TinyTankCharacter->SmartMovement(Destination);
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
