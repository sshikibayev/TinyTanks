


#include "AI/TinyTankAIPawn.h"

#include "AI/PC_AIController.h"
#include <Blueprint/AIBlueprintHelperLibrary.h>

ATinyTankAIPawn::ATinyTankAIPawn()
{
	PrimaryActorTick.bCanEverTick = true;

    AIControllerClass = APC_AIController::StaticClass();
    PC_AIController = CreateDefaultSubobject<APC_AIController>(TEXT("AIController"));
    //PC_AIController = Cast<APC_AIController>(GetController());
}

void ATinyTankAIPawn::MoveToLocation(const FVector& Target)
{
    if (PC_AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PC_AIController is valid: %s"), *Target.ToString());
        //auto SimpleController {Cast<AController>()}
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC_AIController, Target);

       // PC_AIController->MoveToLocation(Target);
    }
}

void ATinyTankAIPawn::BeginPlay()
{
	Super::BeginPlay();

    if (PC_AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PC_AIController is possessed"));
        PC_AIController->Possess(this);
    }
}

void ATinyTankAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATinyTankAIPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

