


#include "TinyTankProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GamePlayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"

ATinyTankProjectile::ATinyTankProjectile()
{
    PrimaryActorTick.bCanEverTick = false;
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

    Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

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

            if (HitParticles && HitSound && HitCameraShakeClass)
            {
                UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
                UGameplayStatics::PlaySoundAtLocation
                (
                    this,
                    HitSound,
                    GetActorLocation()
                );
                GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
            }
        }
        Destroy();
    }
    else
    {
        Destroy();
    }
}

void ATinyTankProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

