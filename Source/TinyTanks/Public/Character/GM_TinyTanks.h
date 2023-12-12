

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GM_TinyTanks.generated.h"

class ATinyTankCharacter;
class APC_TinyTanks;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerJoin);

UCLASS()
class TINYTANKS_API AGM_TinyTanks : public AGameMode
{
	GENERATED_BODY()

public:
    UPROPERTY(Replicated)
    FOnPlayerJoin OnPlayerJoin;

    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    void ActorDied(TObjectPtr<AActor> DeadActor);
    void ActorScored(TObjectPtr<AActor> ScoredActor);

    FTransform GetValidSpawnPoint(const TObjectPtr<ATinyTankCharacter> TinyTankCharacter);

private:
    TObjectPtr<ATinyTankCharacter> TinyTank;
    TObjectPtr<APC_TinyTanks> PC_TinyTanks;
    FName TinyTankTag{ "TinyTank" };
    int PointsForKilling{ 1 };

    bool GetOverlapResult(const FVector& OverlapLocation, TArray<struct FOverlapResult>& OutOverlappedResult);
    void RespawnPlayer(const FTransform& SpawnPoint);
};
