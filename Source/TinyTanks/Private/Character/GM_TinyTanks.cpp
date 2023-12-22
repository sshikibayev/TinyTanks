

#include "Character/GM_TinyTanks.h"

#include "Character/TinyTankCharacter.h"
#include "Character/PC_TinyTanks.h"
#include "Character/PS_TinyTank.h"

AGM_TinyTanks::AGM_TinyTanks()
{
    MakeListOfMaterialsID();
    MakeListOfSpawnPoints();
}

int AGM_TinyTanks::GetMaterialID()
{
    int MaterialID{ 0 };

    if (!ListOfMaterialsID.IsEmpty())
    {
        int IndexOfMaterialsID{ FMath::RandRange(0, ListOfMaterialsID.Num() - 1) };
        MaterialID = ListOfMaterialsID[IndexOfMaterialsID];
        ListOfMaterialsID.RemoveAt(IndexOfMaterialsID);
    }

    return MaterialID;
}

void AGM_TinyTanks::ActorDied(const TObjectPtr<AActor> DeadActor)
{
    if (DeadActor && DeadActor->Tags.Contains(TinyTankTag))
    {
        if (TObjectPtr<ATinyTankCharacter> TinyTankCharacter = Cast<ATinyTankCharacter>(DeadActor))
        {
            TObjectPtr<APlayerController> TinyTankController{ Cast<APlayerController>(TinyTankCharacter->GetController()) };
            TinyTankCharacter->HandleDestruction();
            FTransform ValidSpawnPoint{ GetValidSpawnPoint() };
            TinyTankCharacter = Cast<ATinyTankCharacter>(GetSpawnedCharacter(ValidSpawnPoint));
            PossessTinyTank(TinyTankController, TinyTankCharacter);
        }
    }
}

void AGM_TinyTanks::PostLogin(APlayerController* NewPlayer)
{
    FTransform ValidSpawnPoint{ GetValidSpawnPoint() };
    TObjectPtr<ATinyTankCharacter> TinyTankCharacter = Cast<ATinyTankCharacter>(GetSpawnedCharacter(ValidSpawnPoint));
    PossessTinyTank(NewPlayer, TinyTankCharacter);
}

void AGM_TinyTanks::PossessTinyTank(const TObjectPtr<APlayerController> PossessWith, const TObjectPtr<APawn> PossessWho)
{
    if (PossessWith)
    {
        PossessWith->Possess(PossessWho);
    }
}

void AGM_TinyTanks::ActorScored(const TObjectPtr<APlayerController> ScoredActorController)
{
    if (ScoredActorController)
    {
        UpdateKillingScore(ScoredActorController);
    }
}

void AGM_TinyTanks::MakeListOfMaterialsID()
{
    for (int i{ 0 }; i < TotalMaterials; ++i)
    {
        ListOfMaterialsID.Emplace(i);
    }
}

void AGM_TinyTanks::MakeListOfSpawnPoints()
{
    SpawnPoints.Emplace(FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(1600.0f, 1270.0f, 320.0f)));
    SpawnPoints.Emplace(FTransform(FRotator(0.0f, 180.0f, 0.0f), FVector(1450.0f, 920.0f, 320.0f)));
    SpawnPoints.Emplace(FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(860.0f, 400.0f, 120.0f)));
    SpawnPoints.Emplace(FTransform(FRotator(0.0f, 270.0f, 0.0f), FVector(2390.0f, 2680.0f, 120.0f)));
    SpawnPoints.Emplace(FTransform(FRotator(0.0f, 270.0f, 0.0f), FVector(410.0f, 2840.0f, 120.0f)));
}

void AGM_TinyTanks::UpdateKillingScore(const TObjectPtr<APlayerController> ScoreActorController)
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

FTransform AGM_TinyTanks::GetValidSpawnPoint()
{
    const int MaxTries{ 25 };
    int CountTries{ 0 };
    FTransform CurrentSpawnPoint{ GetSpawnPoint() };

    while (CountTries <= MaxTries)
    {
        bool bFoundValidPoint{ true };
        ++CountTries;

        TArray<struct FOverlapResult> OverlappingResults{};
        CurrentSpawnPoint = GetSpawnPoint();

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

FTransform AGM_TinyTanks::GetSpawnPoint()
{
    if (SpawnPoints.IsEmpty())
    {
        return FTransform::Identity;
    }

    return SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];
}

TObjectPtr<ACharacter> AGM_TinyTanks::GetSpawnedCharacter(const FTransform& SpawnPoint)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    TObjectPtr<ACharacter> SpawnedCharacter = GetWorld()->SpawnActor<ATinyTankCharacter>
        (
            TinyTankCharacterClass,
            SpawnPoint.GetLocation(),
            SpawnPoint.GetRotation().Rotator(),
            SpawnParams
        );

    return SpawnedCharacter;
}