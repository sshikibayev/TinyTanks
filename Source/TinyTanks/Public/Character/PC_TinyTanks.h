

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

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    APC_TinyTanks();

    FORCEINLINE int GetColorID()
    {
        return ColorID;
    }

    void AddToScoreboard(const TObjectPtr<UW_PlayerData> Widget);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* aPawn) override;
    virtual void SetupInputComponent() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void BeginPlayingState() override;

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();

    UFUNCTION()
    void OnFirePressed();
    UFUNCTION()
    void StopFire();

private:
    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold{ 0.3f };

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

    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
    TObjectPtr<APawn> TinyTankPawn;
    FVector CachedDestination;
    float FollowTime{ 0.0f };
    bool bFiringWeapon{ false };
    FTimerHandle FiringTimer;
    int ColorID{ 0 };

    UFUNCTION(Server, Reliable)
    void ServerHandleFire();
    UFUNCTION(Server, Reliable)
    void ServerNavigationMove(const FVector& TargetDestination);
    UFUNCTION(Server, Unreliable)
    void ServerStopMovement();

    void SetColorID();
    void SetupInputMode();
    void ScoreboardInitialization();
    void MakeContinuesMovement();
    void OneTouchAction();
    void PathFindingRefresh();
    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
};
