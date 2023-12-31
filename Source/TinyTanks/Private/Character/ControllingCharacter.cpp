


#include "Character/ControllingCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/PC_TinyTanks.h"
#include "Character/TinyTankCharacter.h"

AControllingCharacter::AControllingCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = TargetArmLength;
    SpringArm->SetRelativeRotation(RelativeRotation);
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetWorldRotation(FRotator(-60.0f, 0.0f, 0.0f));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
    Camera->FieldOfView = 80.0f;

    GetMesh()->SetEnableGravity(false);
    GetMesh()->SetCollisionProfileName(FName(TEXT("OverlapAll")));
    GetCapsuleComponent()->SetEnableGravity(false);
    GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("OverlapAll")));
    GetCharacterMovement()->GravityScale = 0.0f;
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AControllingCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AControllingCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority())
    {
        if (PC_TinyTanks->GetTinyTankCharacter() && !bDoneOnce)
        {
            bDoneOnce = true;
            AttachTo(PC_TinyTanks->GetTinyTankCharacter());
        }
    }
}

void AControllingCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    PC_TinyTanks = Cast<APC_TinyTanks>(GetController());
}

void AControllingCharacter::Destroyed()
{
    SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

    Super::Destroyed();
}

void AControllingCharacter::AttachTo(const TObjectPtr<AActor> Parent)
{
    if (Parent)
    {
        SetActorRotation(Parent->GetActorRotation());
        SetActorLocation(FVector::ZeroVector);
        AttachToActor(Parent, FAttachmentTransformRules::KeepRelativeTransform);
    }
}

