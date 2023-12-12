

#include "Character/GM_TinyTanks.h"

#include "Character/TinyTankCharacter.h"
#include "Character/PC_TinyTanks.h"
#include "Character/PS_TinyTank.h"
#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Widgets/Scoreboard/W_Scoreboard.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AGM_TinyTanks::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (TObjectPtr<APC_TinyTanks> PC_TinyTank{ Cast<APC_TinyTanks>(NewPlayer) })
    {
        OnPlayerJoin.Broadcast();
    }
}

void AGM_TinyTanks::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, OnPlayerJoin);
}

void AGM_TinyTanks::ActorDied(TObjectPtr<AActor> DeadActor)
{
    if (DeadActor && DeadActor->Tags.Contains(TinyTankTag))
    {
        if (TinyTank = Cast<ATinyTankCharacter>(DeadActor))
        {
            if (PC_TinyTanks = Cast<APC_TinyTanks>(TinyTank->GetController()))
            {
                PC_TinyTanks->StopAllMovements();
            }

            TinyTank->HandleDestruction();
            FTransform ValidSpawnPoint{ GetValidSpawnPoint(TinyTank) };
            RespawnPlayer(ValidSpawnPoint);
        }
    }
}

void AGM_TinyTanks::ActorScored(TObjectPtr<AActor> ScoredActor)
{
    /*TObjectPtr<APC_TinyTanks> PC_TinyTankScored = Cast<APC_TinyTanks>(ScoredActor);
    if (PC_TinyTankScored)
    {
        TObjectPtr<APS_TinyTank> PS_TinyTankScored = Cast<APS_TinyTank>(PC_TinyTankScored->GetPawn()->GetPlayerState());
        if (PS_TinyTankScored)
        {
            PS_TinyTankScored->SetPlayerScore(PS_TinyTankScored->GetPlayerScore() + 1);
        }
        PC_TinyTankScored->OnScoreboardUpdate();
    }*/
}

FTransform AGM_TinyTanks::GetValidSpawnPoint(const TObjectPtr<ATinyTankCharacter> TinyTankCharacter)
{
    const int MaxTries{ 25 };
    int CountTries{ 0 };
    FTransform CurrentSpawnPoint{ TinyTankCharacter->GetRespawnPoint() };

    while (CountTries <= MaxTries)
    {
        bool bFoundValidPoint{ true };
        ++CountTries;

        TArray<struct FOverlapResult> OverlappingResults{};
        CurrentSpawnPoint = TinyTankCharacter->GetRespawnPoint();

        if (GetOverlapResult(CurrentSpawnPoint.GetLocation(), OverlappingResults))
        {
            for (const struct FOverlapResult& OverlappingActor : OverlappingResults)
            {
                if (OverlappingActor.GetActor() && OverlappingActor.GetActor()->Tags.Contains(TinyTankTag))
                {
                    bFoundValidPoint = false;
                    break;
                }
            }
        }

        if (bFoundValidPoint)
        {
            CountTries = 0;
            return CurrentSpawnPoint;
        }
    }
    CountTries = 0;

    return CurrentSpawnPoint;
}

bool AGM_TinyTanks::GetOverlapResult(const FVector& OverlapLocation, TArray<struct FOverlapResult>& OutOverlappedResult)
{
    const float SphereRadius{ 100.0f };
    FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::AllObjects);
    FCollisionShape CollisionShape = FCollisionShape::MakeSphere(SphereRadius);
    FCollisionQueryParams CollisionParams;

    bool bHit = GetWorld()->OverlapMultiByObjectType(
        OutOverlappedResult,
        OverlapLocation,
        FQuat::Identity,
        ObjectQueryParams,
        CollisionShape,
        CollisionParams
    );

    return bHit;
}

void AGM_TinyTanks::RespawnPlayer(const FTransform& SpawnPoint)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    TObjectPtr<ATinyTankCharacter> NewSpawnTinyTank{ GetWorld()->SpawnActor<ATinyTankCharacter>
        (
            TinyTank->GetTinyTankCharacterClass(),
            SpawnPoint.GetLocation(),
            SpawnPoint.GetRotation().Rotator(),
            SpawnParams
        ) };

    if (PC_TinyTanks)
    {
        PC_TinyTanks->Possess(NewSpawnTinyTank);
    }
}
