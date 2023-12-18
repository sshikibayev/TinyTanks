

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TinyTankCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class ATinyTankProjectile;
class ATinyTankAICharacter;
class APC_TinyTanks;

UCLASS()
class TINYTANKS_API ATinyTankCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATinyTankCharacter();

    FORCEINLINE TSubclassOf<ATinyTankProjectile> GetProjectileClass() const
    {
        return ProjectileClass;
    };

    FORCEINLINE TSubclassOf<ATinyTankCharacter> GetTinyTankCharacterClass() const
    {
        return TinyTankCharacterClass;
    };

    FORCEINLINE TObjectPtr<USceneComponent> GetProjectileSpawnPoint() const
    {
        return ProjectileSpawnPoint;
    };

    void HandleDestruction();
    FTransform GetRespawnPoint();

    void MoveToLocation(const FVector& Target);

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

    UPROPERTY(EditAnywhere, Category = "Respawn")
    TSubclassOf<ATinyTankCharacter> TinyTankCharacterClass;
    UPROPERTY(EditAnywhere, Category = "Respawn")
    TArray<FTransform> RespawnPoints{};

    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<ATinyTankAICharacter> TinyTankAICharacterClass;
    TObjectPtr<ATinyTankAICharacter> TinyTankAICharacter;


private:
    UPROPERTY(ReplicatedUsing = OnRep_UpdateColor)
    int ColorID{ 0 };

    UPROPERTY(EditAnywhere, Category = "Visual")
    TArray<TSoftObjectPtr<UMaterialInterface>> ListOfAvaliableColors;

    TObjectPtr<APC_TinyTanks> PC_TinyTank;
    const FName TinyTankTag{ "TinyTank" };
    const float TargetArmLength{ 800.0f };
    const FRotator RelativeRotation{ FRotator((-60.0f, 0.0f, 0.0f)) };
    FTransform RespawnPoint;

    UFUNCTION()
    void OnRep_UpdateColor();

    void MoveCharacterToValidSpawnLocation();
    void SetColorID();
    void ApplyMeshColor(const int NewColorID);
    void CreatePawnAI();
    void SetupMovementSettings();
    void CleanPlayerControllersData();
    void DetachComponent();
    void ShowDeathEffects();
};
