

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
class AGM_TinyTanks;
class APS_TinyTank;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetUpdated);

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    FOnWidgetUpdated OnWidgetUpdate;

    APC_TinyTanks();

    void StopAllMovements();
    void AddToScoreboard(const TObjectPtr<UW_PlayerData> Widget);
    void UpdatePlayerScoreOnAServer(const int NewScore);

protected:
    virtual void BeginPlay();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetupInputComponent() override;

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
    TObjectPtr<AGM_TinyTanks> GM_TinyTanks;
    TObjectPtr<APS_TinyTank> PS_TinyTank;
    FVector CachedDestination;
    float FollowTime{ 0.0f };
    int FireCount{ 0 };
    bool bFiringWeapon{ false };
    FTimerHandle FiringTimer;

    UFUNCTION(Server, Reliable)
    void ServerHandleFire();
    UFUNCTION(Server, Reliable)
    void ServerNavigationMove(const FVector& TargetDestionation);
    UFUNCTION(Server, Reliable)
    void ServerStopMovement();
    UFUNCTION(Server, Reliable)
    void ServerSendNameToServer(FName PlayerNickname);

    void UpdatePlayerStateDataOnAServer();
    void SetupInputMode();
    void ScoreboardInitialization();
    void MakeContinuesMovement();
    void OneTouchAction();
    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
};
