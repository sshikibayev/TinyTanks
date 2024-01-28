

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TinyTankProjectile.generated.h"

class UProjectileMovementComponent;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class UParticleSystemComponent;
class AGM_TinyTanks;
class APC_TinyTanks;
class ATinyTankCharacter;

UCLASS()
class TINYTANKS_API ATinyTankProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATinyTankProjectile();

    void SetFireMultiplier(const float FireHoldDuration);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Destroyed() override;

    UPROPERTY(EditInstanceOnly, Category = "Combat")
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
    UPROPERTY(EditAnywhere, Category = "Combat")
    float Damage{ 10.0f };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float MaxDamage{ 100.0f };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float DistanceDivider{ 15.0f };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float InitialSpeed{ 1000.0f };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float CurrentSpeed{ InitialSpeed };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float MaxSpeed{ 3000.0f };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireHoldMultiplier{ 2.0f };
    const float MinFireHoldMultiplier{ 1.0f };
    const float MaxFireHoldDuration{ 3.0f };
    float FireMultiplier{ MinFireHoldMultiplier };

    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<UParticleSystem> HitParticles;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<USoundBase> LaunchSound;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<USoundBase> HitSound;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<UCameraShakeBase> HitCameraShakeClass;

private:
    UPROPERTY()
    TObjectPtr<AGM_TinyTanks> GM_TinyTanks;

    const FName TinyTankTag{ "TinyTank" };
    FVector ProjectileStartLocation{};


    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void InitializeVelocityOfTheProject(const float FireHoldTime);
    float CalculateFinalDamage();
    void AddScore(const TObjectPtr<APC_TinyTanks> KillerController, const float DamageScore);
    float GetCurrentTargetHealth(const TObjectPtr<ATinyTankCharacter> ProjectileTarget);
    float GetKillingScoreMultiplier(const TObjectPtr<ATinyTankCharacter> ProjectileOwner);
    void ShowDeathEffects();
    void PlaySound(TObjectPtr<USoundBase> SoundToPlay);
};
