


#include "Projectile/TinyTankProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/TinyTankCharacter.h"
#include "Character/PC_TinyTanks.h"
#include "Character/PS_TinyTank.h"

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
    if (TObjectPtr<ATinyTankCharacter> ProjectileOwner{ Cast<ATinyTankCharacter>(GetOwner()) })
    {
        if (OtherActor && OtherActor != this && OtherActor != ProjectileOwner && OtherActor->Tags.Contains(TinyTankTag))
        {
            TObjectPtr<AController> MyOwnerInstigator{ ProjectileOwner->GetInstigatorController() };
            TObjectPtr<UClass> DamageTypeClass = UDamageType::StaticClass();

            //Get CurrentTargetHealth before applying a damage.
            const float CurrentTargetHealth{ GetCurrentTargetHealth(Cast<ATinyTankCharacter>(OtherActor)) };
            const float FinalDamage{ CalculateFinalDamage() };

            UGameplayStatics::ApplyDamage
            (
                OtherActor,
                FinalDamage,
                MyOwnerInstigator,
                this,
                DamageTypeClass
            );

            const float FinalScore{ (CurrentTargetHealth - FinalDamage <= 0) ? CurrentTargetHealth * GetKillingScoreMultiplier(ProjectileOwner) : FinalDamage };
            AddScore(Cast<APC_TinyTanks>(ProjectileOwner->GetMainController()), FinalScore);
        }
    }

    ParticleSystemComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Destroy();
}

float ATinyTankProjectile::CalculateFinalDamage()
{
    const float FlewDistance{ static_cast<float>((ProjectileStartLocation - GetActorLocation()).Size()) };
    const float FlewDistanceDamage{ FlewDistance / DistanceDivider };
    const float FinalDamage{ FMath::Clamp(Damage * FireMultiplier + FlewDistanceDamage, 0, MaxDamage) };

    return FinalDamage;
}

float ATinyTankProjectile::GetCurrentTargetHealth(const TObjectPtr<ATinyTankCharacter> ProjectileTarget)
{
    if (ProjectileTarget)
    {
        return ProjectileTarget->GetCurrentHealth();
    }

    return 0.0f;
}

float ATinyTankProjectile::GetKillingScoreMultiplier(const TObjectPtr<ATinyTankCharacter> ProjectileOwner)
{
    TObjectPtr<APlayerController> MainController{ ProjectileOwner->GetMainController() };
    if (MainController && MainController->PlayerState)
    {
        return Cast<APS_TinyTank>(MainController->PlayerState)->GetKillingScoreMultiplier();
    }

    return 0.0f;
}

void ATinyTankProjectile::AddScore(const TObjectPtr<APC_TinyTanks> KillerController, const float DamageScore)
{
    //Called only on server due to GM_TinyTanks exist only on server.
    if (GM_TinyTanks && KillerController)
    {
        GM_TinyTanks->UpdateScore(KillerController, DamageScore);
    }
}
