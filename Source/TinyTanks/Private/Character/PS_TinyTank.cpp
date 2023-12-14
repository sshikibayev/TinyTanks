


#include "Character/PS_TinyTank.h"

#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Widgets/Scoreboard/W_Scoreboard.h"
#include "Character/PC_TinyTanks.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

void APS_TinyTank::BeginPlay()
{
    Super::BeginPlay();

    InitializePlayerDataWidgetToScoreboard();
}

void APS_TinyTank::Destroyed()
{
    Super::Destroyed();
}

void APS_TinyTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, PlayerKillingScore);
    DOREPLIFETIME(ThisClass, PlayerNickname);
}

void APS_TinyTank::InitializePlayerDataWidgetToScoreboard()
{
    BasePlayerController = UGameplayStatics::GetPlayerController(this, 0);
    if (BasePlayerController && BasePlayerController->GetNetMode() != ENetMode::NM_DedicatedServer)
    {
        WBP_PlayerData = CreateWidget<UW_PlayerData>(GetWorld(), PlayerDataClass);
        WBP_PlayerData->SetPlayerName(PlayerNickname);
        WBP_PlayerData->SetPlayerScore(PlayerKillingScore);

        Cast<APC_TinyTanks>(BasePlayerController)->AddToScoreboard(WBP_PlayerData);

        if (auto PC_TinyTank{ Cast<APC_TinyTanks>(GetPlayerController()) })
        {
            PC_TinyTank->UpdatePlayerStateData();
        }
    }
    else if(GetPlayerController() && GetPlayerController()->GetNetMode() == ENetMode::NM_DedicatedServer)
    {
        if (auto PC_TinyTank{ Cast<APC_TinyTanks>(GetPlayerController()) })
        {
            PC_TinyTank->UpdatePlayerStateData();
        }
    }
}

void APS_TinyTank::SetPlayerKillingScore(const int NewScore)
{
    PlayerKillingScore = NewScore;
    WidgetDataUpdate();
}

void APS_TinyTank::SetPlayerNickname(const FText& NewName)
{
    PlayerNickname = NewName;
    WidgetDataUpdate();
}

void APS_TinyTank::OnRep_UpdateScore()
{
    WidgetDataUpdate();
}

void APS_TinyTank::OnRep_UpdateName()
{
    WidgetDataUpdate();
}

void APS_TinyTank::WidgetDataUpdate()
{
    if (WBP_PlayerData)
    {
        WBP_PlayerData->SetPlayerName(PlayerNickname);
        WBP_PlayerData->SetPlayerScore(PlayerKillingScore);
    }
}
