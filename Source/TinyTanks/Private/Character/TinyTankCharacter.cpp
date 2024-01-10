


#include "Character/TinyTankCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Character/PC_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TinyTankAICharacter.h"
#include "AI/PC_AIController.h"
#include "Character/PS_TinyTank.h"
#include "Character/CMC_TinyTank.h"

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
        GetCharacterMovement()->MaxAcceleration = AccelerationSpeed;
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
    }
}

void ATinyTankCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    OnSpawnEventCall();
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

void ATinyTankCharacter::HandleDestruction()
{
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    DetachComponent();
    ShowDeathEffects();
    OnDeadEventCall();
    RemoveAllBondedEvents();

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

void ATinyTankCharacter::BindOnMaterialApplyEvent()
{
    OnSpawn.AddDynamic(this, &ThisClass::OnApplyNewMaterial);
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

void ATinyTankCharacter::OnDeadEventCall()
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
