

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "TinyTankPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class AProjectile;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class UFloatingPawnMovement;

UCLASS()
class TINYTANKS_API ATinyTankPawn : public APawn
{
	GENERATED_BODY()

public:
	ATinyTankPawn();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UCapsuleComponent> CapsuleCollider;

    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<UFloatingPawnMovement> MovementComponent;

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

    /* UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<AProjectile> ProjectileClass;*/
    /* UPROPERTY(EditAnywhere, Category = "Combat")
     TObjectPtr<UParticleSystem> DeathParticles;*/
     /* UPROPERTY(EditAnywhere, Category = "Combat")
      TObjectPtr<USoundBase> DeathSound;
      UPROPERTY(EditAnywhere, Category = "Combat")
      TSubclassOf<UCameraShakeBase> DeathCameraShakeClass;*/
};
