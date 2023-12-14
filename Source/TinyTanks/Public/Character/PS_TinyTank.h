

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "PS_TinyTank.generated.h"

class UW_PlayerData;
class APlayerController;

UCLASS()
class TINYTANKS_API APS_TinyTank : public APlayerState
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    virtual void Destroyed() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    FORCEINLINE int GetPlayerKillScore() const
    {
        return PlayerKillingScore;
    }

    FORCEINLINE FText GetPlayerNickname() const
    {
        return PlayerNickname;
    }

    void SetPlayerKillingScore(const int NewScore);
    void SetPlayerNickname(const FText& NewName);

private:
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_PlayerData> PlayerDataClass;
    TObjectPtr<UW_PlayerData> WBP_PlayerData;
    TObjectPtr<APlayerController> BasePlayerController;

    UPROPERTY(ReplicatedUsing = OnRep_UpdateScore)
    int PlayerKillingScore{ 0 };
    UPROPERTY(ReplicatedUsing = OnRep_UpdateName)
    FText PlayerNickname{ FText::FromString(TEXT("Player name")) };

    UFUNCTION(Server, Reliable)
    void ServerSendNicknameFromClientToServer(const FName& NewPlayerNickname);

    UFUNCTION()
    void WidgetDataUpdate();
    UFUNCTION()
    void OnRep_UpdateScore();
    UFUNCTION()
    void OnRep_UpdateName();

    void InitializePlayerDataWidgetToScoreboard();
};
