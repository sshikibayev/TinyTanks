

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TinyTankAIPawn.generated.h"

class APC_AIController;

UCLASS()
class TINYTANKS_API ATinyTankAIPawn : public APawn
{
	GENERATED_BODY()

public:
	ATinyTankAIPawn();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveToLocation(const FVector& Target);

protected:
	virtual void BeginPlay() override;

private:
    TObjectPtr<APC_AIController> PC_AIController;
};
