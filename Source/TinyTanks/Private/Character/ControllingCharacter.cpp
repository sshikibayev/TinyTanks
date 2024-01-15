


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
    SetNetDormancy(ENetDormancy::DORM_Awake);
    SetReplicateMovement(false);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = TargetArmLength;
    SpringArm->SetRelativeRotation(RelativeRotation);
    SpringArm->bDoCollisionTest = false;
    SpringArm->bEnableCameraLag = true;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->SetWorldRotation(SpringArmRotation);
    SpringArm->PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
    SpringArm->CameraLagMaxTimeStep = 0.05f;
    SpringArm->bUseAttachParentBound = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
    Camera->FieldOfView = FieldOfView;
    Camera->PrimaryComponentTick.bAllowTickOnDedicatedServer = false;

    GetMesh()->SetEnableGravity(false);
    GetMesh()->SetCollisionProfileName(FName(TEXT("OverlapAll")));
    GetMesh()->bReplicatePhysicsToAutonomousProxy = false;

    GetCapsuleComponent()->SetEnableGravity(false);
    GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("OverlapAll")));
    GetCapsuleComponent()->bReplicatePhysicsToAutonomousProxy = false;

    GetCharacterMovement()->GravityScale = GravityScale;
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

}

void AControllingCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    PC_TinyTanks = Cast<APC_TinyTanks>(GetController());

    BindOnCharacterSpawnEvent();
    BindOnCharacterDeathEvent();
}

void AControllingCharacter::Destroyed()
{
    SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    RemoveAllBondedEvents();

    Super::Destroyed();
}


void AControllingCharacter::BindOnCharacterSpawnEvent()
{
    if (PC_TinyTanks)
    {
        PC_TinyTanks->OnCharacterSpawn.AddDynamic(this, &ThisClass::OnTinyTankSpawn);
    }
}

void AControllingCharacter::BindOnCharacterDeathEvent()
{
    if (PC_TinyTanks)
    {
        PC_TinyTanks->OnCharacterDeath.AddDynamic(this, &ThisClass::OnTinyTankDeath);
    }

}

void AControllingCharacter::RemoveAllBondedEvents()
{
    if (PC_TinyTanks)
    {
        PC_TinyTanks->OnCharacterSpawn.RemoveDynamic(this, &ThisClass::OnTinyTankSpawn);
        PC_TinyTanks->OnCharacterDeath.RemoveDynamic(this, &ThisClass::OnTinyTankDeath);
    }
}

void AControllingCharacter::OnTinyTankSpawn()
{
    if (HasAuthority())
    {
        if (PC_TinyTanks)
        {
            AttachTo(PC_TinyTanks->GetTinyTankCharacter());
        }
    }
}

void AControllingCharacter::OnTinyTankDeath()
{
    Detach();
}

void AControllingCharacter::AttachTo(const TObjectPtr<AActor> Parent)
{
    if (Parent && Parent->GetRootComponent())
    {
        AttachToActor(Parent, FAttachmentTransformRules::KeepWorldTransform);
        SetActorLocation(FVector::ZeroVector);
        SetActorRelativeLocation(FVector::ZeroVector);
    }
}

void AControllingCharacter::Detach()
{
    if (HasAuthority())
    {
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }
}
