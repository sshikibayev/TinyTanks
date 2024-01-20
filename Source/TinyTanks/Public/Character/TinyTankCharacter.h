

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
class APS_TinyTank;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS()
class TINYTANKS_API ATinyTankCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATinyTankCharacter(const FObjectInitializer& ObjectInitializer);

    FOnSpawn OnSpawn;
    FOnDeath OnDeath;

    FORCEINLINE TSubclassOf<ATinyTankProjectile> GetProjectileClass() const
    {
        return ProjectileClass;
    };

    FORCEINLINE TObjectPtr<USceneComponent> GetProjectileSpawnPoint() const
    {
        return ProjectileSpawnPoint;
    };

    FORCEINLINE TObjectPtr<APlayerController> GetMainController() const
    {
        return MainController;
    }

    FORCEINLINE void SetMainController(TObjectPtr<APlayerController> NewMainController)
    {
        MainController = NewMainController;
    }

    FORCEINLINE void SetPlayerState(TObjectPtr<APS_TinyTank> NewPlayerState)
    {
        MainPlayerState = NewPlayerState;
    }

    FORCEINLINE TObjectPtr<APS_TinyTank> GetPlayerState() const
    {
        return MainPlayerState;
    }

    FORCEINLINE void SetMaterialID(const int ID)
    {
        MaterialID = ID;
    }

    void ReceiveDestinationForMovement(const FVector& NewDestination);
    void SmartMovement(const FVector& NewDestination);
    void StopMovement();
    void AddFireImpulse(const float ForceMultiplier);

    void MakeFire(const float HoldTime);
    void HandleDestruction();

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual TObjectPtr<ATinyTankProjectile> CreateProjectile(const float ForceMultiplier);

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
    UPROPERTY(EditAnywhere, Category = "Combat")
    int MaxBullets{ 5 };
    int CurrentBullets{ 0 };
    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireImpulseForce{ 750.0f };

private:
    UPROPERTY(ReplicatedUsing = OnRep_UpdateColor)
    int MaterialID{ 0 };

    UPROPERTY(EditAnywhere, Category = "Visual")
    TArray<TSoftObjectPtr<UMaterialInterface>> ListOfAvaliableMaterials;

    UPROPERTY()
    TObjectPtr<APlayerController> MainController;
    UPROPERTY()
    TObjectPtr<APS_TinyTank> MainPlayerState;

    const FName TinyTankTag{ "TinyTank" };
    bool bToggleOnSpawnEvent{ false };

    //CharacterMovementComponent
    const float MaxSpeed{ 1000.0f };
    const float AccelerationSpeed{ 1000.0f };
    const float Mass{ 0.0f };
    FVector DirectionToMove{ FVector::ZeroVector };
    bool bActivateMovement{ false };
    //


    UFUNCTION()
    void OnRep_UpdateColor();
    UFUNCTION()
    void OnApplyNewMaterial();

    void ContinuesMovement();
    void BindOnMaterialApplyEvent();
    void ToggleOnSpawnEvent();
    void InitializeOnSpawnEvent();
    void OnSpawnEventCall();
    void OnDeathEventCall();
    void SetMaterialID();
    void ApplyMaterial(const int NewMaterialID);
    void SetupMovementSettings();
    void DetachComponent();
    void RemoveAllBondedEvents();
    void ShowDeathEffects();
};
