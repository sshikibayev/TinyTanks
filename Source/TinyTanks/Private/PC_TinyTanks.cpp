


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


APC_TinyTanks::APC_TinyTanks()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    CachedDestination = FVector::ZeroVector;
    FollowTime = 0.f;
}

void APC_TinyTanks::BeginPlay()
{
    Super::BeginPlay();

    TinyTankPawn = GetPawn();
    NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
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
}

void APC_TinyTanks::OnSetDestinationTriggered()
{
    FollowTime += GetWorld()->GetDeltaSeconds();

    FHitResult Hit;
    if (GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
    {
        CachedDestination = Hit.Location;
    }

    if (TinyTankPawn)
    {
        FVector WorldDirection = (CachedDestination - TinyTankPawn->GetActorLocation()).GetSafeNormal();

        /*FMatrix RotationMatrix = FRotationMatrix::MakeFromX(WorldDirection);
        FRotator CurrentRotation = TinyTankPawn->GetActorRotation();
        FRotator DesiredRotation = RotationMatrix.Rotator();
        DesiredRotation.Roll = 0;
        DesiredRotation.Pitch = 0;
        FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);*/

        //TinyTankPawn->SetActorRotation(NewRotation);
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

void APC_TinyTanks::OnFirePressed()
{
    TObjectPtr<ATinyTankCharacter> TinyTankCharacter{ Cast<ATinyTankCharacter>(GetPawn()) };
    if (TinyTankCharacter)
    {
        TObjectPtr<ATinyTankProjectile> Projectile = GetWorld()->SpawnActor<ATinyTankProjectile>
            (
                TinyTankCharacter->GetProjectileClass(),
                TinyTankCharacter->GetProjectileSpawnPoint()->GetComponentLocation(),
                TinyTankCharacter->GetProjectileSpawnPoint()->GetComponentRotation()
            );
        Projectile ? Projectile->SetOwner(this) : nullptr;
    }
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
        //if (NavSystem && TinyTankPawn)
        //{
        //    UE_LOG(LogTemp, Warning, TEXT("Nav mesh is valid"));
        //    FPathFindingQuery Query;
        //    Query.StartLocation = TinyTankPawn->GetActorLocation();
        //    Query.EndLocation = CachedDestination;

        //    FPathFindingResult Result = NavSystem->FindPathSync(Query);

        //    if (Result.Path.IsValid())
        //    {
        //        // You can iterate through Result.Path to get individual points on the path
        //        // For simplicity, this example just sets the destination as the last point in the path
        //        //FVector Destination = Result.Path.Get()->GetPathPoints().Last();
        //        if (Result.Path.Get())
        //        {
        //            UE_LOG(LogTemp, Warning, TEXT("Result is valid"));
        //            FVector Destination = Result.Path.Get()->GetGoalLocation();
        //            TinyTankPawn->SetActorLocation(Destination);
        //        }
        //        // Use your own logic to move the pawn to the destination
        //    }
        //}
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
    }
}
