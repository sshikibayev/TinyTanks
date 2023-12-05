// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

void UMenu::SetupMenu(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
    NumPublicConnections = NumberOfPublicConnections;
    MatchType = TypeOfMatch;
    PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    TObjectPtr<UWorld> World = GetWorld();
    if (!World) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (PlayerController)
    {
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(true);
    }

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
    }

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
        MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
        MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
        MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
    }
}

bool UMenu::Initialize()
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (BTN_Host && BTN_Join)
    {
        BTN_Host->OnClicked.AddDynamic(this, &ThisClass::HostBtnClicked);
        BTN_Join->OnClicked.AddDynamic(this, &ThisClass::JoinBtnClicked);
    }

    return true;
}

void UMenu::NativeDestruct()
{
    MenuTearDown();

    Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("bWasSuccessful: true"));

        TObjectPtr<UWorld> World = GetWorld();
        if (!World) return;

        World->ServerTravel(PathToLobby);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Session creation failed"));

        BTN_Host->SetIsEnabled(true);
    }
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
    if (!MultiplayerSessionsSubsystem) return;

    for (auto Result : SessionResults)
    {
        FString SettingsValue;
        Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
        if (SettingsValue == MatchType)
        {
            MultiplayerSessionsSubsystem->JoinSession(Result);
            return;
        }
    }

    if (!bWasSuccessful || SessionResults.Num() <= 0)
    {
        BTN_Join->SetIsEnabled(true);
    }
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if (Subsystem)
    {
        IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

        if (SessionInterface.IsValid())
        {
            FString Address;
            SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

            APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
            if (PlayerController)
            {
                PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
            }
        }
    }

    if (Result != EOnJoinSessionCompleteResult::Success)
    {
        BTN_Join->SetIsEnabled(true);
    }
}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMenu::HostBtnClicked()
{
    BTN_Host->SetIsEnabled(false);

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
    }
}

void UMenu::JoinBtnClicked()
{
    BTN_Join->SetIsEnabled(false);

    if (MultiplayerSessionsSubsystem)
    {
        MultiplayerSessionsSubsystem->FindSessions(10000);
    }
}

void UMenu::MenuTearDown()
{
    RemoveFromParent();

    TObjectPtr<UWorld> World = GetWorld();
    if (!World) return;
    APlayerController* PlayerController = World->GetFirstPlayerController();

    if (PlayerController)
    {
        FInputModeGameOnly InputModeData;
        PlayerController->SetInputMode(InputModeData);
        PlayerController->SetShowMouseCursor(false);
    }
}
