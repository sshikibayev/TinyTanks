

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "PS_TinyTank.generated.h"

class UW_PlayerData;

UCLASS()
class TINYTANKS_API APS_TinyTank : public APlayerState
{
    GENERATED_BODY()

public:
    FORCEINLINE int GetPlayerScore() const
    {
        return PlayerScore;
    }
    FORCEINLINE FName GetPlayerNickname() const
    {
        return PlayerNickname;
    }
    FORCEINLINE int GetColorID() const
    {
        return ColorID;
    }
    FORCEINLINE float GetKillingScoreMultiplier() const
    {
        return KillingScoreMultiplier;
    }

    FORCEINLINE void SetColorID(const int NewColorID)
    {
        ColorID = NewColorID;
    }

    FORCEINLINE void SetPlayerNickname(const FName& NewName)
    {
        PlayerNickname = NewName;
        OnRep_UpdateName();
    }

    FORCEINLINE void SetPlayerScore(const float NewScore)
    {
        PlayerScore = NewScore;
        OnRep_UpdateScore();
    }

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_PlayerData> PlayerDataClass;

    UPROPERTY(EditAnywhere, Category = "Score")
    float KillingScoreMultiplier{ 2.5f };

    UPROPERTY()
    TObjectPtr<UW_PlayerData> WBP_PlayerData;

    UPROPERTY(ReplicatedUsing = OnRep_UpdateName)
    FName PlayerNickname{ TEXT("Player name") };
    UPROPERTY(ReplicatedUsing = OnRep_UpdateScore)
    float PlayerScore{ 0 };

    int ColorID{ 0 };

    UFUNCTION(Server, Reliable)
    void ServerSendNicknameFromClientToServer(const FName& NewPlayerNickname);

    UFUNCTION()
    void OnRep_UpdateScore();
    UFUNCTION()
    void OnRep_UpdateName();

    UFUNCTION()
    void WidgetDataUpdate();

    void InitializePlayerNickname();
    void InitializePlayerDataWidgetToScoreboard();
};
