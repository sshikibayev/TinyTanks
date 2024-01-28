

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "AC_Health.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthUpdate, float, Health);

class AGM_TinyTanks;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TINYTANKS_API UAC_Health : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_Health();

    FORCEINLINE float GetMaxHealth()
    {
        return MaxHealth;
    }

    FORCEINLINE float GetCurrentHealth()
    {
        return Health;
    }

    UPROPERTY(Replicated);
    FOnHealthUpdate OnHealthUpdate;

protected:
	virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

private:
    UPROPERTY(EditAnywhere);
    float MaxHealth{ 100.0f };

    UPROPERTY(Replicated);
    float Health{ 0.0f };

    TObjectPtr<AGM_TinyTanks> GM_TinyTanks;

    UFUNCTION()
    void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);
};
