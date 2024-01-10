

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CMC_TinyTank.generated.h"

UCLASS()
class TINYTANKS_API UCMC_TinyTank : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
    UCMC_TinyTank();

protected:
    virtual void BeginPlay() override;

    virtual void SmoothCorrection(const FVector& OldLocation, const FQuat& OldRotation, const FVector& NewLocation, const FQuat& NewRotation) override;

    virtual bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

};
