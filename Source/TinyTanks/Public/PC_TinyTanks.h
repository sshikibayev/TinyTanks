

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PC_TinyTanks.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;
class UNavigationSystemV1;

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    APC_TinyTanks();

    /** Time Threshold to know if it was a short press */
    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold{ 3.0f };
    UPROPERTY(EditAnywhere, Category = Movement)
    float RotationSpeed{ 5.0f };

    /** FX Class that we will spawn when clicking */
    UPROPERTY(EditAnywhere, Category = Input)
    TObjectPtr<UNiagaraSystem> FXCursor;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputMappingContext> IMC_TinyTanks;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_SetDestinationByClick;

    UPROPERTY(EditAnywhere, Category = Input)
    TSoftObjectPtr<UInputAction> IA_Fire;

protected:
    virtual void BeginPlay();
    virtual void SetupInputComponent() override;

    void OnInputStarted();
    void OnSetDestinationTriggered();
    void OnSetDestinationReleased();
    void OnTouchTriggered();
    void OnTouchReleased();

    UFUNCTION()
    void LaunchFire();
    UFUNCTION()
    void StopFire();

    void OnFirePressed();

private:
    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
    TObjectPtr<UNavigationSystemV1> NavSystem;

    FVector CachedDestination;
    float FollowTime; // For how long it has been pressed

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRate{ 0.25f };

    bool bFiringWeapon{ false };
    FTimerHandle FiringTimer;

    UFUNCTION(Server, Reliable)
    void HandleFire();

    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
    void OneTouchAction();
};
