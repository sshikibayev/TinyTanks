

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

    virtual void Tick(float DeltaTime) override;
protected:

	virtual void BeginPlay() override;

private:
    UPROPERTY(EditInstanceOnly, Category = "Combat")
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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
};
