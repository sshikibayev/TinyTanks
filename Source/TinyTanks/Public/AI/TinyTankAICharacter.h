

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TinyTankAICharacter.generated.h"

class APC_AIController;


UCLASS()
class TINYTANKS_API ATinyTankAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATinyTankAICharacter();

    void MoveToLocation(const FVector& Target);
    void PossessCreatedAIPawn(const TObjectPtr<APawn> AIPawn);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(EditAnywhere, Category = "AI")
    TSubclassOf<APC_AIController> PC_AIControllerClass;
    TObjectPtr<APC_AIController> PC_AIController;
};
