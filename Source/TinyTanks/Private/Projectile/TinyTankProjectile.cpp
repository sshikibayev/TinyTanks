


#include "Projectile/TinyTankProjectile.h"

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

    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = MaxSpeed;
}

void ATinyTankProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (GetLocalRole() == ROLE_Authority)
    {
        Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
    }

    PlaySound(LaunchSound);
}

void ATinyTankProjectile::Destroyed()
{
    ShowDeathEffects();

    Super::Destroyed();
}

void ATinyTankProjectile::ShowDeathEffects()
{
    if (HitParticles && HitSound && HitCameraShakeClass)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
        PlaySound(HitSound);
        GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
    }
}
void ATinyTankProjectile::PlaySound(TObjectPtr<USoundBase> SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation
        (
            this,
            SoundToPlay,
            GetActorLocation()
        );
    }
}

void ATinyTankProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (TObjectPtr<AActor> MyOwner{ GetOwner() })
    {
        TObjectPtr<AController> MyOwnerInstigator{ MyOwner->GetInstigatorController() };
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
        }
    }

    ParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

    Destroy();
}