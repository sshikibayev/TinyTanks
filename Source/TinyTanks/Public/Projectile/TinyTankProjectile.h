

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TinyTankProjectile.generated.h"

class UProjectileMovementComponent;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class UParticleSystemComponent;

UCLASS()
class TINYTANKS_API ATinyTankProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATinyTankProjectile();

protected:
	virtual void BeginPlay() override;
    virtual void Destroyed() override;

    UPROPERTY(EditInstanceOnly, Category = "Combat")
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
    UPROPERTY(EditAnywhere)
    float Damage{ 1.0f };
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
    const float InitialSpeed{ 1000.0f };
    const float MaxSpeed{ 3000.0f };

    void PlaySound(TObjectPtr<USoundBase> SoundToPlay);

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void ShowDeathEffects();
};
