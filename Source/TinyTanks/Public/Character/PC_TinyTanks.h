

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

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    APC_TinyTanks();

    FORCEINLINE void SetTinyTankCharacter(const TObjectPtr<ATinyTankCharacter> NewTinyTankCharacter)
    {
        TinyTankCharacter = NewTinyTankCharacter;
    }

    FORCEINLINE TObjectPtr<ATinyTankCharacter> GetTinyTankCharacter()
    {
        return TinyTankCharacter;
    }

    void AddToScoreboard(const TObjectPtr<UW_PlayerData> Widget);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();

    UFUNCTION()
    void OnFirePressed();
    UFUNCTION()
    void StopFire();

private:
    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold{ 0.25f };

    bool bContinuesMovementHold{ false };

    UPROPERTY(EditAnywhere, Category = Input)
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputMappingContext> IMC_TinyTanks;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_SetDestinationByClick;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_Fire;

    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_Scoreboard> ScoreboardClass;
    TObjectPtr<UW_Scoreboard> WBP_Scoreboard;

    UPROPERTY(EditAnywhere, Category = Combat)
    float FireRate{ 0.25f };

    UPROPERTY()
    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
    UPROPERTY(Replicated)
    TObjectPtr<ATinyTankCharacter> TinyTankCharacter;

    FVector CachedDestination;
    float FollowTime{ 0.0f };
    bool bFiringWeapon{ false };
    FTimerHandle FiringTimer;
    int ColorID{ 0 };

    UFUNCTION(Server, Reliable)
    void ServerHandleFire();
    UFUNCTION(Server, Unreliable)
    void ServerStopMovement();
    UFUNCTION(Server, Unreliable)
    void ServerMakeContinuesMovement(const FVector& Destination);
    UFUNCTION(Server, Unreliable)
    void ServerSmartMove(const FVector& Destination);

    void ContinuesMovement(const FVector& Destination);
    void SmartMovement(const FVector& Destination);
    void SetupInputMode();
    void ScoreboardInitialization();
    void OneTouchAction();
    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
    void DrawDebugRayFromMouseClick();
};
