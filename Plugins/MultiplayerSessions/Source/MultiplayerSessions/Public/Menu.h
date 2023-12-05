// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

class UButton;
class UMultiplayerSessionsSubsystem;

UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetupMenu(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = "FreeForAll", FString LobbyPath = "/Game/ThirdPerson/Maps/Lobby");

protected:
    virtual bool Initialize() override;
    virtual void NativeDestruct() override;

    /**
    * Callbacks for the custom delegates on the UMultiplayerSessionsSubsystem.
    */
    UFUNCTION()
    void OnCreateSession(bool bWasSuccessful);
    void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccsessful);
    void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
    UFUNCTION()
    void OnStartSession(bool bWasSuccessful);
    UFUNCTION()
    void OnDestroySession(bool bWasSuccessful);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BTN_Host;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BTN_Join;

    int32 NumPublicConnections{ 4 };
    FString MatchType{ "FreeForAll" };
    FString PathToLobby{ TEXT("")};

    //The subsystem designed to handle all online session functionality.
    TObjectPtr<UMultiplayerSessionsSubsystem> MultiplayerSessionsSubsystem;

    UFUNCTION()
    void HostBtnClicked();
    UFUNCTION()
    void JoinBtnClicked();

    void MenuTearDown();
};
