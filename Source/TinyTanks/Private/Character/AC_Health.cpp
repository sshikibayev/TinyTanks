


#include "Character/AC_Health.h"

#include "Character/GM_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"


UAC_Health::UAC_Health()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_Health::BeginPlay()
{
	Super::BeginPlay();

    Health = MaxHealth;

    if (GetOwner()->GetLocalRole() == ROLE_Authority)
    {
        GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ThisClass::DamageTaken);
    }

    GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this));
}

void UAC_Health::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAC_Health::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
    if (Damage > 0.0f)
    {
        Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
        if (Health <= 0 && GM_TinyTanks)
        {
            GM_TinyTanks->ActorDied(DamagedActor);
        }
    }
}
