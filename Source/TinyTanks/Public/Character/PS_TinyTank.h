

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PS_TinyTank.generated.h"

class UW_PlayerData;
class UW_Scoreboard;

UCLASS()
class TINYTANKS_API APS_TinyTank : public APlayerState
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    FORCEINLINE int GetPlayerScore() const
    {
        return PlayerScore;
    }

    FORCEINLINE FText GetPlayerName() const
    {
        return PlayerName;
    }

    FORCEINLINE TObjectPtr<UW_PlayerData> GetPlayerData() const
    {
        return WBP_PlayerData;
    }

    void SetPlayerScore(const int NewScore);
    void SetPlayerName(const FText& NewName);
    void SetPlayerData(const TObjectPtr<UW_PlayerData> NewPlayerData);

private:
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_PlayerData> PlayerDataClass;
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_Scoreboard> ScoreboardClass;
    TObjectPtr<UW_Scoreboard> WBP_Scoreboard;

    FTimerHandle TestTimer;

    UPROPERTY(ReplicatedUsing = UpdateScore)
    int PlayerScore{ 0 };
    UPROPERTY(Replicated)
    FText PlayerName{ FText::FromString(TEXT("Player name")) };

    TObjectPtr<UW_PlayerData> WBP_PlayerData;

    UFUNCTION()
    void WidgetDataUpdate();
    UFUNCTION()
    void UpdateScore();
};
