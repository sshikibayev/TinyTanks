

#include "Character/GM_TinyTanks.h"

#include "Character/TinyTankCharacter.h"
#include "Character/PC_TinyTanks.h"
#include "Character/PS_TinyTank.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

void AGM_TinyTanks::ActorDied(TObjectPtr<AActor> DeadActor)
{
    if (DeadActor && DeadActor->Tags.Contains(TinyTankTag))
    {
        if (TinyTank = Cast<ATinyTankCharacter>(DeadActor))
        {
            ForceMovementStop();

            TinyTank->HandleDestruction();
            FTransform ValidSpawnPoint{ GetValidSpawnPoint(TinyTank) };
            RespawnPlayer(ValidSpawnPoint);
        }
    }
}

void AGM_TinyTanks::ActorScored(TObjectPtr<AActor> ScoredActor)
{
    if (ScoredActor)
    {
        UpdateKillingScore(Cast<APC_TinyTanks>(ScoredActor));
    }
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

void AGM_TinyTanks::ForceMovementStop()
{
    if (PC_TinyTanks = Cast<APC_TinyTanks>(TinyTank->GetController()))
    {
        PC_TinyTanks->StopAllMovements();
    }
}

void AGM_TinyTanks::UpdateKillingScore(const TObjectPtr<APC_TinyTanks> ScoreActorController)
{
    if (ScoreActorController)
    {
        TObjectPtr<APS_TinyTank> PS_TinyTankScored = Cast<APS_TinyTank>(ScoreActorController->PlayerState);
        if (PS_TinyTankScored)
        {
            PS_TinyTankScored->SetPlayerKillingScore(PS_TinyTankScored->GetPlayerKillScore() + PointsForKilling);
        }
    }
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
