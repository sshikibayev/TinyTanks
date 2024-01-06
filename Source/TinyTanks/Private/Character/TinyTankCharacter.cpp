


#include "Character/TinyTankCharacter.h"

#include "Character/PC_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TinyTankAICharacter.h"
#include "AI/PC_AIController.h"
#include "Character/PS_TinyTank.h"
#include "Net/UnrealNetwork.h"

ATinyTankCharacter::ATinyTankCharacter()
{
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

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
    }

    Tags.Emplace(TinyTankTag);

    OnSpawn.AddDynamic(this, &ThisClass::OnApplyNewMaterial);
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

void ATinyTankCharacter::InitializeOnSpawnEvent()
{
    if (bToggleOnSpawnEvent && MainController && MainController->GetOwner() && MainPlayerState)
    {
        bToggleOnSpawnEvent = !bToggleOnSpawnEvent;
        OnSpawn.Broadcast();
    }
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
