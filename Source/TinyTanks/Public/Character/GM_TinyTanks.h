

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
    AGM_TinyTanks();

    void ActorDied(const TObjectPtr<AActor> DeadActor);
    void UpdateScore(const TObjectPtr<APC_TinyTanks> ScoredController, const float FinalScore);
    int GetMaterialID();

protected:
    virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage);
    virtual void PostLogin(APlayerController* NewPlayer) override;

private:
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TArray<FTransform> SpawnPoints{};
    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<ATinyTankCharacter> TinyTankCharacterClass;

    FName TinyTankTag{ "TinyTank" };
    int TotalMaterials{ 4 };
    TArray<int> ListOfMaterialsID;

    void CharacterInitialization(const TObjectPtr<APlayerController> MainController);
    void MakeListOfMaterialsID();
    void MakeListOfSpawnPoints();
    bool GetOverlapResult(const FVector& OverlapLocation, TArray<struct FOverlapResult>& OutOverlappedResult);
    FTransform GetValidSpawnPoint();
    FTransform GetSpawnPoint();
    TObjectPtr<ACharacter> GetSpawnedCharacter(const FTransform& SpawnPoint);
};
