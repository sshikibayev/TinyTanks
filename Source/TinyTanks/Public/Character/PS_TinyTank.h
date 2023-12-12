

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "PS_TinyTank.generated.h"

class UW_PlayerData;
class UW_Scoreboard;
class APlayerController;

UCLASS()
class TINYTANKS_API APS_TinyTank : public APlayerState
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    FORCEINLINE int GetPlayerScore() const
    {
        return PlayerScore;
    }

    FORCEINLINE FText GetPlayerName() const
    {
        return PlayerName;
    }

    void SetPlayerScore(const int NewScore);
    void SetPlayerName(const FText& NewName);

private:
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_PlayerData> PlayerDataClass;
    TObjectPtr<UW_PlayerData> WBP_PlayerData;
    TObjectPtr<APlayerController> BasePlayerController;

    UPROPERTY(ReplicatedUsing = UpdateScore)
    int PlayerScore{ 0 };
    UPROPERTY(Replicated)
    FText PlayerName{ FText::FromString(TEXT("Player name")) };

    UFUNCTION()
    void WidgetDataUpdate();
    UFUNCTION()
    void UpdateScore();
};
