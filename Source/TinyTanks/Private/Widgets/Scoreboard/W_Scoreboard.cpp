


#include "Widgets/Scoreboard/W_Scoreboard.h"

#include "Widgets/Scoreboard/W_PlayerData.h"
#include "Components/VerticalBox.h"

void UW_Scoreboard::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UW_Scoreboard::NativeConstruct()
{
    Super::NativeConstruct();
}

void UW_Scoreboard::NativeDestruct()
{
    Super::NativeDestruct();
}

void UW_Scoreboard::AddWidget(const TObjectPtr<UW_PlayerData> Widget)
{
    if (VB_List && Widget)
    {
        VB_List->AddChildToVerticalBox(Widget);
    }
}