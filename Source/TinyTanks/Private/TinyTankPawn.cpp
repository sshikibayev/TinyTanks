


#include "TinyTankPawn.h"

#include "Kismet/GamePlayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ATinyTankPawn::ATinyTankPawn()
{
    CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule collider"));
    SetRootComponent(CapsuleCollider);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(CapsuleCollider);
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = 800.f;
    SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base mesh"));
    BaseMeshComponent->SetupAttachment(CapsuleCollider);

    TurretMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret mesh"));
    TurretMeshComponent->SetupAttachment(BaseMeshComponent);

    ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile spawn point"));
    ProjectileSpawnPoint->SetupAttachment(TurretMeshComponent);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
    MovementComponent->SetUpdatedComponent(CapsuleCollider);

    MovementComponent->bConstrainToPlane = true;
    MovementComponent->bSnapToPlaneAtStart = true;
}

void ATinyTankPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATinyTankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATinyTankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}
