

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "ControllingCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class APC_TinyTanks;

UCLASS()
class TINYTANKS_API AControllingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AControllingCharacter();

protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void PossessedBy(AController* NewController) override;

    virtual void Destroyed() override;

private:
    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<USpringArmComponent> SpringArm;
    UPROPERTY(EditAnywhere, Category = "Components")
    TObjectPtr<UCameraComponent> Camera;

    TObjectPtr<APC_TinyTanks> PC_TinyTanks;
    const float TargetArmLength{ 800.0f };
    const FRotator RelativeRotation{ FRotator((-60.0f, 0.0f, 0.0f)) };

    UFUNCTION()
    void OnAttachTo();

    void AttachTo(const TObjectPtr<AActor> Parent);
};
