


#include "AI/PC_AIController.h"
#include "Character/GM_TinyTanks.h"
#include "Kismet/GameplayStatics.h"

void APC_AIController::SmartMoveToLocation(const FVector& Direction)
{
    MoveToLocation(Direction);
}

void APC_AIController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    SetColorID();
}

void APC_AIController::BeginPlay()
{
    Super::BeginPlay();

    PathFindingRefresh();
}

void APC_AIController::SetColorID()
{
    if (auto GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this)))
    {
        ColorID = GM_TinyTanks->GetMaterialID();
    }
}

void APC_AIController::PathFindingRefresh()
{
    TObjectPtr<UPathFollowingComponent> PathFollowingComp = FindComponentByClass<UPathFollowingComponent>();
    if (PathFollowingComp)
    {
        PathFollowingComp->UpdateCachedComponents();
    }
}
