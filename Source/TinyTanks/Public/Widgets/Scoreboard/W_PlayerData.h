

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
    void SetPlayerName(const FName& NewName);
    void SetPlayerScore(float NewScore);

    FORCEINLINE FName GetPlayerName() const
    {
        return Name;
    }

    FORCEINLINE int GetPlayerScore() const
    {
        return Score;
    }

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr <UTextBlock> TB_Name;
    UPROPERTY(meta = (BindWidget))
    TObjectPtr <UTextBlock> TB_Score;

    FName Name{ TEXT("Default") };
    float Score{ 0.0f };
};
