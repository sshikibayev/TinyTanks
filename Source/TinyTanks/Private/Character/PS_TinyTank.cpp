


#include "Character/PS_TinyTank.h"

#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Widgets/Scoreboard/W_Scoreboard.h"
#include "Character/PC_TinyTanks.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Character/GI_TinyTanks.h"
#include "Character/GM_TinyTanks.h"

#include "Net/UnrealNetwork.h"

void APS_TinyTank::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (auto GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this)))
    {
        ColorID = GM_TinyTanks->GetMaterialID();
    }
}

void APS_TinyTank::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SetPlayerScore(0);
    }

    InitializePlayerNickname();
    InitializePlayerDataWidgetToScoreboard();
}


void APS_TinyTank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, PlayerScore);
    DOREPLIFETIME(ThisClass, PlayerNickname);
}

void APS_TinyTank::InitializePlayerNickname()
{
    if (GetNetMode() == ENetMode::NM_ListenServer)
    {
        if (TObjectPtr<UGI_TinyTanks> GameInstance = Cast<UGI_TinyTanks>(GetGameInstance()))
        {
            SetPlayerNickname(GameInstance->GetPlayerNickname());
        }
    }

    if (GetNetMode() == ENetMode::NM_Client)
    {
        if (TObjectPtr<UGI_TinyTanks> GameInstance = Cast<UGI_TinyTanks>(GetGameInstance()))
        {
            ServerSendNicknameFromClientToServer(GameInstance->GetPlayerNickname());
        }
    }
}

void APS_TinyTank::InitializePlayerDataWidgetToScoreboard()
{
    if (!IsRunningDedicatedServer())
    {
        WBP_PlayerData = CreateWidget<UW_PlayerData>(GetWorld(), PlayerDataClass);
        WBP_PlayerData->SetPlayerName(PlayerNickname);
        WBP_PlayerData->SetPlayerScore(PlayerScore);

        if (auto LocalPC_TinyTank{ Cast<APC_TinyTanks>(UGameplayStatics::GetPlayerController(this, 0)) })
        {
            LocalPC_TinyTank->AddToScoreboard(WBP_PlayerData);
        }
    }
}

void APS_TinyTank::OnRep_UpdateScore()
{
    WidgetDataUpdate();
}

void APS_TinyTank::OnRep_UpdateName()
{
    WidgetDataUpdate();
}

void APS_TinyTank::ServerSendNicknameFromClientToServer_Implementation(const FName& NewPlayerNickname)
{
    SetPlayerNickname(NewPlayerNickname);
}

void APS_TinyTank::WidgetDataUpdate()
{
    if (WBP_PlayerData)
    {
        WBP_PlayerData->SetPlayerName(PlayerNickname);
        WBP_PlayerData->SetPlayerScore(PlayerScore);
    }
}