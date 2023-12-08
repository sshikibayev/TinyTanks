


#include "Character/TinyTankCharacter.h"

#include "Kismet/GamePlayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include <Character/PC_TinyTanks.h>

ATinyTankCharacter::ATinyTankCharacter()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = 800.f;
    SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
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

    Tags.Emplace(TEXT("TinyTank"));
}

void ATinyTankCharacter::BeginPlay()
{
    Super::BeginPlay();

    SetActorTransform(GetRespawnPoint());

    if (!HasAuthority())
    {
        TObjectPtr<APC_TinyTanks> PC_TinyTank{ Cast<APC_TinyTanks>(GetController()) };
        if (PC_TinyTank)
        {
            TObjectPtr<UPathFollowingComponent> PathFollowingComp = PC_TinyTank->FindComponentByClass<UPathFollowingComponent>();
            if (PathFollowingComp)
            {
                PathFollowingComp->UpdateCachedComponents();
            }
        }
    }

}

void ATinyTankCharacter::Destroyed()
{
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    BaseMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    TurretMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    ProjectileSpawnPoint->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

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

    Super::Destroyed();
}

void ATinyTankCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ATinyTankCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATinyTankCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, RespawnPoint);
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
