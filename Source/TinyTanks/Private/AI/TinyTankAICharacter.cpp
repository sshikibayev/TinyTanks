


#include "AI/TinyTankAICharacter.h"

#include "AI/PC_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ATinyTankAICharacter::ATinyTankAICharacter()
{
   /* PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;*/
}

void ATinyTankAICharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ATinyTankAICharacter::Tick(float DeltaTime)
{
}

void ATinyTankAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
}
