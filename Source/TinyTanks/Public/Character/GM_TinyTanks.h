

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GM_TinyTanks.generated.h"

class ATinyTankCharacter;
class APC_AIController;

UCLASS()
class TINYTANKS_API AGM_TinyTanks : public AGameMode
{
    GENERATED_BODY()

public:
    AGM_TinyTanks();

    int GetMaterialID();
    void ActorDied(const TObjectPtr<AActor> DeadActor);
    void ActorScored(const TObjectPtr<APlayerController> ScoredActorController);

protected:
    virtual void PostLogin(APlayerController* NewPlayer) override;

private:
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TArray<FTransform> SpawnPoints{};
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<ATinyTankCharacter> TinyTankCharacterClass;

    FName TinyTankTag{ "TinyTank" };
    int PointsForKilling{ 1 };
    int TotalMaterials{ 4 };
    TArray<int> ListOfMaterialsID;

    void MakeListOfMaterialsID();
    void MakeListOfSpawnPoints();
    void UpdateKillingScore(const TObjectPtr<APlayerController> ScoreActorController);
    bool GetOverlapResult(const FVector& OverlapLocation, TArray<struct FOverlapResult>& OutOverlappedResult);
    FTransform GetValidSpawnPoint();
    FTransform GetSpawnPoint();
    TObjectPtr<ACharacter> GetSpawnedCharacter(const FTransform& SpawnPoint);
};
