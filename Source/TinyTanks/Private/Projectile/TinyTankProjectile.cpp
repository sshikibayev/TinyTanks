


#include "Projectile/TinyTankProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GM_TinyTanks.h"


ATinyTankProjectile::ATinyTankProjectile()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = false;
    SetReplicateMovement(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    SetRootComponent(Mesh);

    ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle system"));
    ParticleSystemComponent->SetupAttachment(Mesh);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile movement component"));
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = MaxSpeed;
}

void ATinyTankProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();

}

void ATinyTankProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
        GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this));
        ProjectileStartLocation = GetActorLocation();
    }

    PlaySound(LaunchSound);
}

void ATinyTankProjectile::Destroyed()
{
    ShowDeathEffects();

    if (HasAuthority() && Mesh)
    {
        Mesh->OnComponentHit.RemoveDynamic(this, &ThisClass::OnHit);
    }

    Super::Destroyed();
}

void ATinyTankProjectile::SetFireMultiplier(const float FireHoldDuration)
{
    const float FireHoldTime{ FMath::Clamp(FireHoldDuration, MinFireHoldMultiplier, MaxFireHoldDuration) };
    FireMultiplier = FireHoldTime * FireHoldMultiplier;

    InitializeVelocityOfTheProject(FireHoldTime);
}

void ATinyTankProjectile::InitializeVelocityOfTheProject(const float FireHoldTime)
{
    if (ProjectileMovementComponent)
    {
        CurrentSpeed = FMath::Clamp(InitialSpeed * FireHoldTime, InitialSpeed, MaxSpeed);
        ProjectileMovementComponent->Velocity = GetActorForwardVector() * CurrentSpeed;
    }
}

//*Killer - is a APlayerController;
void ATinyTankProjectile::AddScoreForKilling(const TObjectPtr<AActor> Killer)
{
    if (GetOwner() && GetOwner()->HasAuthority() && GM_TinyTanks)
    {
        GM_TinyTanks->ActorScored(Cast<APlayerController>(Killer));
    }
}

void ATinyTankProjectile::ShowDeathEffects()
{
    if (HitParticles && HitSound && HitCameraShakeClass)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
        PlaySound(HitSound);
        if (GetWorld()->GetFirstPlayerController())
        {
            GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitCameraShakeClass);
        }
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

        if (OtherActor && OtherActor != this && OtherActor != MyOwner && OtherActor->Tags.Contains(TinyTankTag))
        {
            float FlewDistance{ static_cast<float>((ProjectileStartLocation - GetActorLocation()).Size()) };
            float FlewDistanceDamage{ FlewDistance / DistanceDivider };
            float FinalDamage{ FMath::Clamp(Damage * FireMultiplier + FlewDistanceDamage, 0, MaxDamage) };

            UGameplayStatics::ApplyDamage
            (
                OtherActor,
                FinalDamage,
                MyOwnerInstigator,
                this,
                DamageTypeClass
            );


            //TODO think to move it to the AC_Health
            //Here is passing not AActor but APlayerController;
            AddScoreForKilling(MyOwner);
        }
    }

    ParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

    Destroy();
}