

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Scoreboard.generated.h"

class UVerticalBox;
class UW_PlayerData;
class UTextBlock;

UCLASS()
class TINYTANKS_API UW_Scoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
    void AddWidget(const TObjectPtr<UW_PlayerData> Widget);


protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> VB_List;
};
