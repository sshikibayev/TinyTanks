


#include "Character/PS_TinyTank.h"

#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Widgets/Scoreboard/W_Scoreboard.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PC_TinyTanks.h"

void APS_TinyTank::BeginPlay()
{
    Super::BeginPlay();

    if (UGameplayStatics::GetPlayerController(this, 0)->GetNetMode() == ENetMode::NM_Client || UGameplayStatics::GetPlayerController(this, 0)->GetNetMode() == ENetMode::NM_ListenServer)
    {
        WBP_PlayerData = CreateWidget<UW_PlayerData>(GetWorld(), PlayerDataClass);
        WBP_PlayerData->SetPlayerName(PlayerName);
        WBP_PlayerData->SetPlayerScore(PlayerScore);

        Cast<APC_TinyTanks>(UGameplayStatics::GetPlayerController(this, 0))->AddToScoreboard(WBP_PlayerData);

        if (auto PC_TinyTank{ Cast<APC_TinyTanks>(GetPlayerController()) })
        {
            PC_TinyTank->OnScoreUpdated.AddDynamic(this, &ThisClass::WidgetDataUpdate);
        }
    }
}

void APS_TinyTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, PlayerScore);
    DOREPLIFETIME(ThisClass, PlayerName);
}

void APS_TinyTank::SetPlayerScore(const int NewScore)
{
    PlayerScore = NewScore;
}

void APS_TinyTank::SetPlayerName(const FText& NewName)
{
    PlayerName = NewName;
}

void APS_TinyTank::SetPlayerData(const TObjectPtr<UW_PlayerData> NewPlayerData)
{
    if (NewPlayerData)
    {
        WBP_PlayerData = NewPlayerData;
    }
}

void APS_TinyTank::UpdateScore()
{
    WidgetDataUpdate();
}

void APS_TinyTank::WidgetDataUpdate()
{
    if (WBP_PlayerData)
    {
        WBP_PlayerData->SetPlayerScore(PlayerScore);
    }
}