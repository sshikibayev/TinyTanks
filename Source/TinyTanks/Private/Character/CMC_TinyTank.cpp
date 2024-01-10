


#include "Character/CMC_TinyTank.h"

UCMC_TinyTank::UCMC_TinyTank()
{

}

void UCMC_TinyTank::BeginPlay()
{
    Super::BeginPlay();

    ForceClientAdjustment();
}

void UCMC_TinyTank::SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation)
{
    Super::SmoothCorrection(OldLocation, OldRotation, NewLocation, NewRotation);

}

bool UCMC_TinyTank::ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
    bool Result{ Super::ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientWorldLocation, RelativeClientLocation, ClientMovementBase, ClientBaseBoneName, ClientMovementMode) };

    return Result;
}
