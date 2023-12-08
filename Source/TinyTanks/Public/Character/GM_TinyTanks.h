

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

protected:
    virtual void BeginPlay();

private:
    TObjectPtr<ATinyTankCharacter> TinyTank;
    TObjectPtr<APC_TinyTanks> PC_TinyTanks;

    void RespawnPlayer();
};
