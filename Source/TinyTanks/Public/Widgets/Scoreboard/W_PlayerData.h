

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PlayerData.generated.h"

class UTextBlock;

UCLASS()
class TINYTANKS_API UW_PlayerData : public UUserWidget
{
    GENERATED_BODY()
public:
    void SetPlayerName(const FText& NewName);
    void SetPlayerScore(int NewScore);

    FORCEINLINE FText GetPlayerName() const
    {
        return Name;
    }

    FORCEINLINE int GetPlayerScore() const
    {
        return Score;
    }

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr <UTextBlock> TB_Name;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr <UTextBlock> TB_Score;

    FText Name{ FText::FromString(TEXT("Default")) };
    int Score{ 0 };
};
