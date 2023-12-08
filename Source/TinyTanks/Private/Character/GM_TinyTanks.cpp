

#include "Character/GM_TinyTanks.h"

#include "Character/TinyTankCharacter.h"
#include "Character/PC_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"


void AGM_TinyTanks::ActorDied(TObjectPtr<AActor> DeadActor)
{
    if (DeadActor && DeadActor->Tags.Contains("TinyTank"))
    {
        if (TinyTank = Cast<ATinyTankCharacter>(DeadActor))
        {
            PC_TinyTanks = Cast<APC_TinyTanks>(TinyTank->GetController());
            TinyTank->HandleDestruction();

            RespawnPlayer();
        }
    }
}

void AGM_TinyTanks::BeginPlay()
{

}

void AGM_TinyTanks::RespawnPlayer()
{
    FTransform RespawnPoint{ TinyTank->GetRespawnPoint() };

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    TObjectPtr<ATinyTankCharacter> NewSpawnTinyTank{ GetWorld()->SpawnActor<ATinyTankCharacter>
        (
            TinyTank->GetTinyTankCharacterClass(),
            RespawnPoint.GetLocation(),
            RespawnPoint.GetRotation().Rotator(),
            SpawnParams
        )};

    if (PC_TinyTanks)
    {
        PC_TinyTanks->Possess(NewSpawnTinyTank);
    }
}
