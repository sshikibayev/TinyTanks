

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "GI_TinyTanks.generated.h"

UCLASS()
class TINYTANKS_API UGI_TinyTanks : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    FORCEINLINE void SetPlayerNickname(const FName NewPlayerNickname)
    {
        PlayerNickname = NewPlayerNickname;
    }

    UFUNCTION(BlueprintCallable)
    FORCEINLINE FName GetPlayerNickname() const
    {
        return PlayerNickname;
    }

private:
    FName PlayerNickname{ "Unknown" };
};
