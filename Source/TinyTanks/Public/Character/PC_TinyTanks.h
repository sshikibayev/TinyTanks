

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PC_TinyTanks.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;
class UW_PlayerData;
class UW_Scoreboard;
class ATinyTankCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    APC_TinyTanks();

    //Calls every time when new character spawned.
    FOnCharacterSpawn OnCharacterSpawn;
    //Calls every time when character died.
    FOnCharacterDeath OnCharacterDeath;

    FORCEINLINE TObjectPtr<ATinyTankCharacter> GetTinyTankCharacter()
    {
        return TinyTankCharacter;
    }

    void SetTinyTankCharacter(const TObjectPtr<ATinyTankCharacter> NewTinyTankCharacter);
    void AddToScoreboard(const TObjectPtr<UW_PlayerData> Widget);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void OnPossess(APawn* InPawn) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Destroyed() override;

    UFUNCTION()
    virtual void CharacterSpawned();
    UFUNCTION()
    virtual void CharacterDead();
    UFUNCTION()
    void OnFireHold();
    UFUNCTION()
    void OnFirePressed();
    UFUNCTION()
    void StopFire();

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();

private:
    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold{ 0.25f };
    UPROPERTY(EditAnywhere, Category = Input)
    TObjectPtr<UNiagaraSystem> FXCursor;
    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputMappingContext> IMC_TinyTanks;
    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_SetDestinationByClick;
    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_Fire;

    UPROPERTY(EditAnywhere, Category = Input)
    float MovementHoldRate{ 0.01f };
    FTimerHandle MovementHoldingTimer;
    bool bMovementHolding{ false };
    float FollowTime{ 0.0f };

    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_Scoreboard> ScoreboardClass;

    //Move to a TinyTankCharacter
    UPROPERTY(EditAnywhere, Category = Combat)
    float FireRate{ 0.5f };
    FTimerHandle FiringTimer;
    bool bFiringWeapon{ false };
    float FireHoldTime{ 0.0f };

    UPROPERTY()
    TObjectPtr<UW_Scoreboard> WBP_Scoreboard;
    UPROPERTY()
    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;

    TObjectPtr<ATinyTankCharacter> TinyTankCharacter;
    FVector CachedDestination;
    int ColorID{ 0 };

    UFUNCTION(Server, Reliable)
    void ServerHandleFire(const float NewFireHoldTime);

    //Movement RPC's
    UFUNCTION(Server, Unreliable)
    void ServerSetDestination(const FVector& Destination);
    UFUNCTION(Server, Unreliable)
    void ServerSmartMove(const FVector& Destination);
    UFUNCTION(Server, Unreliable)
    void ServerStopMovement();

    UFUNCTION()
    void RefreshMovementHold();

    void SpawnedCharacterInitialization();
    void BindOnSpawnEvent();
    void BindOnDeadEvent();
    void CalculateDestination();
    void SetDestinationForTheTinyTank(const FVector& Destination);
    void SmartMovement(const FVector& Destination);
    void StopMovement();
    void AddFireImpulse(const float ForceMultiplier);
    void SetupInputMode();
    void ScoreboardInitialization();
    void OneTouchAction();
    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
    void RemoveAllBondedEvents();
    void DrawDebugRayFromMouseClick();
};
