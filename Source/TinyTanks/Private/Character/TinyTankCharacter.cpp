


#include "Character/TinyTankCharacter.h"

#include "Character/PC_TinyTanks.h"
#include "Character/GM_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ATinyTankCharacter::ATinyTankCharacter()
{
    SetupMovementSettings();

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = TargetArmLength;
    SpringArm->SetRelativeRotation(RelativeRotation);
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

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

    Tags.Emplace(TinyTankTag);
}

void ATinyTankCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (auto GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this)))
    {
        const FTransform SpawnPoint{ GM_TinyTanks->GetValidSpawnPoint(this) };
        SetActorTransform(SpawnPoint);
    }
}

void ATinyTankCharacter::Destroyed()
{
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    DetachComponent();
    ShowDeathEffects();

    Super::Destroyed();
}

void ATinyTankCharacter::SetupMovementSettings()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATinyTankCharacter::DetachComponent()
{
    SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
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

void ATinyTankCharacter::HandleDestruction()
{
    Destroy();
}

FTransform ATinyTankCharacter::GetRespawnPoint()
{
    if (HasAuthority())
    {
        FTransform RandomRespawnPoint = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num() - 1)];
        RespawnPoint = RandomRespawnPoint;
    }

    return RespawnPoint;
}
