

#include "GM_TinyTanks.h"

#include "TinyTankCharacter.h"
#include "PC_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"


void AGM_TinyTanks::ActorDied(TObjectPtr<AActor> DeadActor)
{
    if (DeadActor && DeadActor->Tags.Contains("TinyTank"))
    {
        if (TinyTank = Cast<ATinyTankCharacter>(DeadActor))
        {
            TinyTank->HandleDestruction();

            RespawnPlayer();
        }

        if (PC_TinyTanks)
        {
            UE_LOG(LogTemp, Warning, TEXT("PC_Is valid"));
            //PC_TinyTank->SetPlayerEnabledState(false);
        }
    }
}

void AGM_TinyTanks::BeginPlay()
{
    //TinyTank = Cast<ATinyTankCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    PC_TinyTanks = Cast<APC_TinyTanks>(UGameplayStatics::GetPlayerController(this, 0));
}

void AGM_TinyTanks::RespawnPlayer()
{
    FTransform RespawnPoint{ TinyTank->GetRespawnPoint() };
    UE_LOG(LogTemp, Warning, TEXT("Respawn point: %s"), *RespawnPoint.ToString());

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    TObjectPtr<ATinyTankCharacter> NewSpawnTinyTank{ GetWorld()->SpawnActor<ATinyTankCharacter>(TinyTank->GetTinyTankCharacterClass(), RespawnPoint.GetLocation(), RespawnPoint.GetRotation().Rotator(), SpawnParams)};

    UE_LOG(LogTemp, Warning, TEXT("Respawned NewSpawnTinyTank: %s"), *NewSpawnTinyTank.GetName());
}
