

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_Health.generated.h"

class AGM_TinyTanks;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TINYTANKS_API UAC_Health : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_Health();

protected:
	virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere);
    float MaxHealth{ 1.0f };
    float Health{ 0.0f };
    TObjectPtr<AGM_TinyTanks> GM_TinyTanks;

    UFUNCTION()
    void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);
};
