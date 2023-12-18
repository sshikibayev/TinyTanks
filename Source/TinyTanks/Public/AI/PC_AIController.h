

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PC_AIController.generated.h"

UCLASS()
class TINYTANKS_API APC_AIController : public AAIController
{
	GENERATED_BODY()

protected:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    virtual void OnPossess(APawn* aPawn) override;
};
