

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "TinyTankCharacter.generated.h"

class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class ATinyTankProjectile;
class ATinyTankAICharacter;
class APC_AIController;

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

    FORCEINLINE TObjectPtr<USceneComponent> GetProjectileSpawnPoint() const
    {
        return ProjectileSpawnPoint;
    };

    void HandleDestruction();

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<ATinyTankAICharacter> TinyTankAICharacterClass;
    TObjectPtr<ATinyTankAICharacter> TinyTankAICharacter;

private:
    UPROPERTY(ReplicatedUsing = OnRep_UpdateColor)
    int MaterialID{ 0 };

    UPROPERTY(EditAnywhere, Category = "Visual")
    TArray<TSoftObjectPtr<UMaterialInterface>> ListOfAvaliableMaterials;

    const FName TinyTankTag{ "TinyTank" };

    UFUNCTION()
    void OnRep_UpdateColor();

    void SetColorID();
    void ApplyMeshesColor(const int NewMaterialID);
    void SetupMovementSettings();
    void DetachComponent();
    void ShowDeathEffects();
};
