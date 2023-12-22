


#include "AI/TinyTankAICharacter.h"

#include "AI/PC_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

ATinyTankAICharacter::ATinyTankAICharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;

    PC_AIController = CreateDefaultSubobject<APC_AIController>(TEXT("AIController"));
    AIControllerClass = PC_AIControllerClass;
    PC_AIController->SetOwner(this);
}

void ATinyTankAICharacter::MoveToLocation(const FVector& Target)
{
    if (PC_AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PC_AIController is valid: %s"), *PC_AIController->GetName());

        /* auto SimpleController{ Cast<AController>(GetController()) };
         if (SimpleController)
         {
             UE_LOG(LogTemp, Warning, TEXT("SimpleController is valid: %s"), *SimpleController->GetName());

             UAIBlueprintHelperLibrary::SimpleMoveToLocation(SimpleController, Target);
         }

        FVector WorldDirection = (Target - GetActorLocation()).GetSafeNormal();
        AddMovementInput(WorldDirection);*/

        if (PC_AIController->MoveToLocation(Target))
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation is valid"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation is NOT valid"));
        }
    }
}

void ATinyTankAICharacter::PossessCreatedAIPawn(const TObjectPtr<APawn> AIPawn)
{
    if (PC_AIController && AIPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("PC_AIController is possessed: %s"), *AIPawn->GetName());
        PC_AIController->Possess(AIPawn);
    }
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
