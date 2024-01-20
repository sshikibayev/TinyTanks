


#include "Character/TinyTankCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Character/PC_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TinyTankAICharacter.h"
#include "AI/PC_AIController.h"
#include "Character/PS_TinyTank.h"
#include "Character/CMC_TinyTank.h"
#include "Projectile/TinyTankProjectile.h"
#include "Net/UnrealNetwork.h"

ATinyTankCharacter::ATinyTankCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCMC_TinyTank>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    SetupMovementSettings();

    BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base mesh"));
    BaseMeshComponent->SetupAttachment(GetRootComponent());

    TurretMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret mesh"));
    TurretMeshComponent->SetupAttachment(BaseMeshComponent);

    ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile spawn point"));
    ProjectileSpawnPoint->SetupAttachment(TurretMeshComponent);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
        GetCharacterMovement()->MaxWalkSpeedCrouched = MaxSpeed;
        GetCharacterMovement()->MaxAcceleration = AccelerationSpeed;
        GetCharacterMovement()->Mass = Mass;
        GetCharacterMovement()->bTouchForceScaledToMass = false;
        GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Walking;
        GetCharacterMovement()->DefaultWaterMovementMode = EMovementMode::MOVE_None;
        GetCharacterMovement()->BrakingDecelerationWalking = 0.0f;
        GetCharacterMovement()->bTickBeforeOwner = false;

        //Networking
        GetCharacterMovement()->NetworkSmoothingMode = ENetworkSmoothingMode::Linear;
        GetCharacterMovement()->bNetworkSkipProxyPredictionOnNetUpdate = true;
        GetCharacterMovement()->NetworkNoSmoothUpdateDistance = 50.0f;
        GetCharacterMovement()->NetworkMinTimeBetweenClientAckGoodMoves = 0.0f;
        GetCharacterMovement()->NetworkMinTimeBetweenClientAdjustments = 0.0f;
    }

    if (GetCapsuleComponent())
    {
        GetCapsuleComponent()->bDynamicObstacle = true;
    }

    Tags.Emplace(TinyTankTag);

    BindOnMaterialApplyEvent();
}

void ATinyTankCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    CurrentBullets = MaxBullets;
}

void ATinyTankCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ATinyTankCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (HasAuthority())
    {
        InitializeOnSpawnEvent();
        ContinuesMovement();
    }
}

void ATinyTankCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    ToggleOnSpawnEvent();
}

void ATinyTankCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, MaterialID);
}

void ATinyTankCharacter::OnRep_UpdateColor()
{
    ApplyMaterial(MaterialID);
}

void ATinyTankCharacter::ReceiveDestinationForMovement(const FVector& NewDestination)
{
    DirectionToMove = (NewDestination - GetActorLocation()).GetSafeNormal();
    bActivateMovement = true;
}

void ATinyTankCharacter::SmartMovement(const FVector& NewDestination)
{
    TObjectPtr<APC_AIController> PC_AI{ Cast<APC_AIController>(GetController()) };
    if (PC_AI)
    {
        PC_AI->SmartMoveToLocation(NewDestination);
    }
}

void ATinyTankCharacter::StopMovement()
{
    if (GetController())
    {
        GetController()->StopMovement();
        bActivateMovement = false;
    }
}

void ATinyTankCharacter::MakeFire(const float HoldTime)
{
    if (CurrentBullets > 0)
    {
        CreateProjectile(HoldTime);
        AddFireImpulse(HoldTime);
        CurrentBullets = FMath::Clamp(--CurrentBullets, 0, MaxBullets);
    }
}

TObjectPtr<ATinyTankProjectile> ATinyTankCharacter::CreateProjectile(const float ForceMultiplier)
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Instigator = GetInstigator();
    SpawnParameters.Owner = this;

    TObjectPtr<ATinyTankProjectile> Projectile = GetWorld()->SpawnActor<ATinyTankProjectile>
        (
            GetProjectileClass(),
            GetProjectileSpawnPoint()->GetComponentLocation(),
            GetProjectileSpawnPoint()->GetComponentRotation(),
            SpawnParameters
        );

    if (Projectile)
    {
        Projectile->SetFireMultiplier(ForceMultiplier);
    }

    return Projectile;
}

void ATinyTankCharacter::AddFireImpulse(const float ForceMultiplier)
{
    const FVector Backward{ -GetActorForwardVector() };
    const FVector FinalForceImpulse{ Backward * ForceMultiplier * FireImpulseForce };
    LaunchCharacter(FinalForceImpulse, false, false);
}

void ATinyTankCharacter::HandleDestruction()
{
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    DetachComponent();
    ShowDeathEffects();
    OnDeathEventCall();
    RemoveAllBondedEvents();
    StopMovement();

    Destroy();
}

void ATinyTankCharacter::OnApplyNewMaterial()
{
    if (HasAuthority())
    {
        SetMaterialID();
    }

    if (GetNetMode() != ENetMode::NM_DedicatedServer)
    {
        ApplyMaterial(MaterialID);
    }
}


void ATinyTankCharacter::ContinuesMovement()
{
    if (bActivateMovement)
    {
        //Doesn't work at the client side. Only from a server.
        AddMovementInput(DirectionToMove);
    }
}

void ATinyTankCharacter::BindOnMaterialApplyEvent()
{
    OnSpawn.AddDynamic(this, &ThisClass::OnApplyNewMaterial);
}

void ATinyTankCharacter::ToggleOnSpawnEvent()
{
    //We toggle this bool, to call InitializeOnSpawnEvent() on Tick function,
   //to invoke an event OnSpawnEventCall().
    bToggleOnSpawnEvent = true;
}

void ATinyTankCharacter::InitializeOnSpawnEvent()
{
    if (bToggleOnSpawnEvent && MainController && MainController->GetPawn() && MainPlayerState)
    {
        bToggleOnSpawnEvent = !bToggleOnSpawnEvent;
        OnSpawnEventCall();
    }
}

void ATinyTankCharacter::OnSpawnEventCall()
{
    OnSpawn.Broadcast();
}

void ATinyTankCharacter::OnDeathEventCall()
{
    OnDeath.Broadcast();
}

void ATinyTankCharacter::SetMaterialID()
{
    if (MainPlayerState)
    {
        MaterialID = MainPlayerState->GetColorID();
    }
}

void ATinyTankCharacter::ApplyMaterial(const int NewMaterialID)
{
    if (BaseMeshComponent && TurretMeshComponent)
    {
        BaseMeshComponent->SetMaterial(0, ListOfAvaliableMaterials[NewMaterialID].LoadSynchronous());
        TurretMeshComponent->SetMaterial(0, ListOfAvaliableMaterials[NewMaterialID].LoadSynchronous());
    }
}

void ATinyTankCharacter::SetupMovementSettings()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATinyTankCharacter::DetachComponent()
{
    BaseMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    TurretMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    ProjectileSpawnPoint->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

void ATinyTankCharacter::RemoveAllBondedEvents()
{
    OnSpawn.RemoveDynamic(this, &ThisClass::OnApplyNewMaterial);
    OnDeath.RemoveDynamic(this, &ThisClass::OnApplyNewMaterial);

    OnSpawn.Clear();
}

void ATinyTankCharacter::ShowDeathEffects()
{
    if (DeathParticles && DeathSound && DeathCameraShakeClass && GetWorld()->GetFirstPlayerController())
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());

        UGameplayStatics::PlaySoundAtLocation
        (
            this,
            DeathSound,
            GetActorLocation()
        );

        GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
    }
}
