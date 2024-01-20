


#include "Character/AC_Health.h"

#include "Character/GM_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"


UAC_Health::UAC_Health()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAC_Health::BeginPlay()
{
	Super::BeginPlay();

    Health = MaxHealth;

    if (GetOwner()->HasAuthority())
    {
        GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ThisClass::DamageTaken);
        GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this));
    }
}

void UAC_Health::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    if (GetOwner()->HasAuthority())
    {
        GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::DamageTaken);
    }

    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UAC_Health::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
    if (Damage > 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Damage received: %f"), Damage);

        Health = FMath::Clamp(Health - Damage, 0, MaxHealth);

        UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

        if (Health <= 0 && GM_TinyTanks)
        {
            GM_TinyTanks->ActorDied(DamagedActor);
        }
    }
}
