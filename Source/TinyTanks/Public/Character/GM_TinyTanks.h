

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GM_TinyTanks.generated.h"

class ATinyTankCharacter;
class APC_TinyTanks;

UCLASS()
class TINYTANKS_API AGM_TinyTanks : public AGameMode
{
	GENERATED_BODY()

public:
    void ActorDied(TObjectPtr<AActor> DeadActor);
    void ActorScored(TObjectPtr<AActor> ScoredActor);

    FTransform GetValidSpawnPoint(const TObjectPtr<ATinyTankCharacter> TinyTankCharacter);

private:
    TObjectPtr<ATinyTankCharacter> TinyTank;
    TObjectPtr<APC_TinyTanks> PC_TinyTanks;
    FName TinyTankTag{ "TinyTank" };
    int PointsForKilling{ 1 };

    void UpdateKillingScore(const TObjectPtr<APC_TinyTanks> ScoreActorController);
    void ForceMovementStop();
    bool GetOverlapResult(const FVector& OverlapLocation, TArray<struct FOverlapResult>& OutOverlappedResult);
    void RespawnPlayer(const FTransform& SpawnPoint);
};
