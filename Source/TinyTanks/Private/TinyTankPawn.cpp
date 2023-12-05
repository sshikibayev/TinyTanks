


#include "TinyTankPawn.h"

#include "Kismet/GamePlayStatics.h"
#include "Components/CapsuleComponent.h"

ATinyTankPawn::ATinyTankPawn()
{
	PrimaryActorTick.bCanEverTick = true;

    CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule collider"));
    SetRootComponent(CapsuleCollider);

    BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base mesh"));
    BaseMeshComponent->SetupAttachment(CapsuleCollider);

    TurretMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret mesh"));
    TurretMeshComponent->SetupAttachment(BaseMeshComponent);

    ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile spawn point"));
    ProjectileSpawnPoint->SetupAttachment(TurretMeshComponent);
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

