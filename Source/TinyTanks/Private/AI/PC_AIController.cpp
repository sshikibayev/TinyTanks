


#include "AI/PC_AIController.h"

void APC_AIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    UE_LOG(LogTemp, Warning, TEXT("OnMoveCompleted is finished: %s"), *Result.ToString());
}

void APC_AIController::OnPossess(APawn* aPawn)
{
    if (aPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Possessed inside AI controller: %s"), *aPawn->GetName());
    }
}
