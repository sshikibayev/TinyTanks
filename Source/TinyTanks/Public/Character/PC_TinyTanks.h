

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "PC_TinyTanks.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS()
class TINYTANKS_API APC_TinyTanks : public APlayerController
{
    GENERATED_BODY()

public:
    APC_TinyTanks();
    void StopAllMovements();

    UPROPERTY(EditAnywhere, Category = Input)
    float ShortPressThreshold{ 3.0f };

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

    UFUNCTION()
    void OnFirePressed();
    UFUNCTION()
    void StopFire();

private:
    TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem;
    FVector CachedDestination;
    float FollowTime{ 0.0f };
    int FireCount{ 0 };
    bool bFiringWeapon{ false };
    FTimerHandle FiringTimer;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRate{ 0.25f };

    UFUNCTION(Server, Reliable)
    void Server_HandleFire();
    UFUNCTION(Server, Reliable)
    void Server_NavigationMove(const FVector& TargetDestionation);
    UFUNCTION(Server, Reliable)
    void Server_StopMovement();

    void SetupInputMode();
    void MakeContinuesMovement();
    void OneTouchAction();
    void PrepareInputSubsystem();
    void AddingMappingContext(TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem, const TSoftObjectPtr<UInputMappingContext> MappingContext);
    void BindInputActions();
};
