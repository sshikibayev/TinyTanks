


#include "Widgets/Scoreboard/W_PlayerData.h"

#include "Components/TextBlock.h"

void UW_PlayerData::SetPlayerName(const FName& NewName)
{
    Name = NewName;
    if (TB_Name)
    {
        TB_Name->SetText(FText::FromString(Name.ToString()));
    }
}

void UW_PlayerData::SetPlayerScore(float NewScore)
{
    Score = NewScore;
    if (TB_Score)
    {
        TB_Score->SetText(FText::AsNumber(Score));;
    }
}

void UW_PlayerData::NativeConstruct()
{
    Super::NativeConstruct();

    SetPlayerName(Name);
    SetPlayerScore(Score);
}