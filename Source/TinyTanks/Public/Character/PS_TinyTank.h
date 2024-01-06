

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
    FORCEINLINE int GetPlayerKillScore() const
    {
        return PlayerKillingScore;
    }
    FORCEINLINE FText GetPlayerNickname() const
    {
        return PlayerNickname;
    }
    FORCEINLINE int GetColorID() const
    {
        return ColorID;
    }
    FORCEINLINE void SetColorID(const int NewColorID)
    {
        ColorID = NewColorID;
    }

    void SetPlayerKillingScore(const int NewScore);
    void SetPlayerNickname(const FText& NewName);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(EditAnywhere, Category = Widget)
    TSubclassOf<UW_PlayerData> PlayerDataClass;

    UPROPERTY()
    TObjectPtr<UW_PlayerData> WBP_PlayerData;
    UPROPERTY()
    TObjectPtr<APlayerController> BasePlayerController;

    UPROPERTY(ReplicatedUsing = OnRep_UpdateScore)
    int PlayerKillingScore{ 0 };
    UPROPERTY(ReplicatedUsing = OnRep_UpdateName)
    FText PlayerNickname{ FText::FromString(TEXT("Player name")) };

    int ColorID{ 0 };

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
