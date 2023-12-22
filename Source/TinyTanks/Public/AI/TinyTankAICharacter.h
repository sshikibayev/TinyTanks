

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TinyTankAICharacter.generated.h"

class APC_AIController;


UCLASS()
class TINYTANKS_API ATinyTankAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATinyTankAICharacter();

    void MoveToLocation(const FVector& Target);
    void PossessCreatedAIPawn(const TObjectPtr<APawn> AIPawn);


protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<APC_AIController> PC_AIControllerClass;
    TObjectPtr<APC_AIController> PC_AIController;
};
