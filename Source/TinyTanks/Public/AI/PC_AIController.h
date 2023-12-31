

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PC_AIController.generated.h"

UCLASS()
class TINYTANKS_API APC_AIController : public AAIController
{
    GENERATED_BODY()

public:
    FORCEINLINE int GetColorID()
    {
        return ColorID;
    }

    void SmartMoveToLocation(const FVector& Direction);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;

private:
    int ColorID{ 0 };

    void SetColorID();
    void PathFindingRefresh();
};
