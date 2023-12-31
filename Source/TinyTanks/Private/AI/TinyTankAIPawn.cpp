


#include "AI/TinyTankAIPawn.h"

#include "AI/PC_AIController.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>

ATinyTankAIPawn::ATinyTankAIPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATinyTankAIPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATinyTankAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATinyTankAIPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
