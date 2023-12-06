

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TinyTankCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AProjectile;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class ATinyTankProjectile;

UCLASS()
class TINYTANKS_API ATinyTankCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATinyTankCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    TSubclassOf<ATinyTankProjectile> GetProjectileClass();
    TObjectPtr<USceneComponent> GetProjectileSpawnPoint();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> BaseMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> TurretMeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<USceneComponent> ProjectileSpawnPoint;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<ATinyTankProjectile> ProjectileClass;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<UParticleSystem> DeathParticles;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TObjectPtr<USoundBase> DeathSound;
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;
};
