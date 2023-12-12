

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "W_Scoreboard.generated.h"

class UVerticalBox;
class UW_PlayerData;

UCLASS()
class TINYTANKS_API UW_Scoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
    void AddWidget(const TObjectPtr<UW_PlayerData> Widget);

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> VB_List;
};
