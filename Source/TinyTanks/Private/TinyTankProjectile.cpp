


#include "TinyTankProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"


ATinyTankProjectile::ATinyTankProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(Mesh);

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle system"));
    ParticleSystemComponent->SetupAttachment(Mesh);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));

    ProjectileMovementComponent->InitialSpeed = 4000.0f;
    ProjectileMovementComponent->MaxSpeed = 8000.0f;

}

void ATinyTankProjectile::BeginPlay()
{
	Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
    }

    if (LaunchSound)
    {
        UGameplayStatics::PlaySoundAtLocation
        (
            this,
            LaunchSound,
            GetActorLocation()
        );
    }
}

void ATinyTankProjectile::Destroyed()
{
    UE_LOG(LogTemp, Warning, TEXT("Projectile destroyed"));

    if (HitParticles && HitSound && HitCameraShakeClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("Effects applied on a Projectile"));

        UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
        UGameplayStatics::PlaySoundAtLocation
        (
            this,
            HitSound,
            GetActorLocation()
        );
        GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);

    }

    Super::Destroyed();
}

void ATinyTankProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    TObjectPtr<AActor> MyOwner = GetOwner();
    if (MyOwner)
    {
        TObjectPtr<AController> MyOwnerInstigator = MyOwner->GetInstigatorController();
        auto DamageTypeClass = UDamageType::StaticClass();

        if (OtherActor && OtherActor != this && OtherActor != MyOwner)
        {
            UGameplayStatics::ApplyDamage
            (
                OtherActor,
                Damage,
                MyOwnerInstigator,
                this,
                DamageTypeClass
            );

            UE_LOG(LogTemp, Warning, TEXT("Damage applied to: %s"), *OtherActor->GetName());
        }
    }

    Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    ParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Destroy();
}

void ATinyTankProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

