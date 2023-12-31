

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

protected:
	virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
