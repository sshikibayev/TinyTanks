


#include "Character/TinyTankCharacter.h"

#include "Character/PC_TinyTanks.h"
#include "Character/GM_TinyTanks.h"
#include "Kismet/GamePlayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/TinyTankAICharacter.h"
#include "Net/UnrealNetwork.h"

ATinyTankCharacter::ATinyTankCharacter()
{
    SetupMovementSettings();

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->TargetArmLength = TargetArmLength;
    SpringArm->SetRelativeRotation(RelativeRotation);
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base mesh"));
    BaseMeshComponent->SetupAttachment(GetRootComponent());

    TurretMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret mesh"));
    TurretMeshComponent->SetupAttachment(BaseMeshComponent);

    ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile spawn point"));
    ProjectileSpawnPoint->SetupAttachment(TurretMeshComponent);

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    Tags.Emplace(TinyTankTag);
}

void ATinyTankCharacter::BeginPlay()
{
    Super::BeginPlay();

    MoveCharacterToValidSpawnLocation();

    /* if (HasAuthority())
     {
         CreatePawnAI();
     }*/
}

void ATinyTankCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    SetColorID();
    if (GetNetMode() != ENetMode::NM_DedicatedServer)
    {
        ApplyMeshesColor(ColorID);
    }
}

void ATinyTankCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, ColorID);
}

void ATinyTankCharacter::OnRep_UpdateColor()
{
    ApplyMeshesColor(ColorID);
}

void ATinyTankCharacter::MoveCharacterToValidSpawnLocation()
{
    if (auto GM_TinyTanks = Cast<AGM_TinyTanks>(UGameplayStatics::GetGameMode(this)))
    {
        const FTransform SpawnPoint{ GM_TinyTanks->GetValidSpawnPoint(this) };
        SetActorTransform(SpawnPoint);
    }
}

void ATinyTankCharacter::SetColorID()
{
    if (TObjectPtr<APC_TinyTanks> PC_TinyTank = Cast<APC_TinyTanks>(GetController()))
    {
        ColorID = PC_TinyTank->GetColorID();
    }
}

void ATinyTankCharacter::HandleDestruction()
{
    CleanPlayerControllersData();

    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);

    DetachComponent();
    ShowDeathEffects();

    Destroy();
}

FTransform ATinyTankCharacter::GetRespawnPoint()
{
    if (HasAuthority())
    {
        FTransform RandomRespawnPoint = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num() - 1)];
        RespawnPoint = RandomRespawnPoint;
    }

    return RespawnPoint;
}

void ATinyTankCharacter::MoveToLocation(const FVector& Target)
{
    if (TinyTankAICharacter)
    {
        TinyTankAICharacter->MoveToLocation(Target);
    }
}

void ATinyTankCharacter::ApplyMeshesColor(const int NewColorID)
{
    if (BaseMeshComponent && TurretMeshComponent)
    {
        BaseMeshComponent->SetMaterial(0, ListOfAvaliableColors[NewColorID].LoadSynchronous());
        TurretMeshComponent->SetMaterial(0, ListOfAvaliableColors[NewColorID].LoadSynchronous());
    }
}

void ATinyTankCharacter::CreatePawnAI()
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnParameters.Owner = this;
    TinyTankAICharacter = GetWorld()->SpawnActor<ATinyTankAICharacter>
        (
            TinyTankAICharacterClass,
            GetActorLocation() + FVector(80.0f, 80.0f, 0.0f),
            GetActorRotation(),
            SpawnParameters
        );

    if (TinyTankAICharacter)
    {
        TinyTankAICharacter->PossessCreatedAIPawn(TinyTankAICharacter);
        //TinyTankAICharacter->GetRootComponent()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
    }
}

void ATinyTankCharacter::SetupMovementSettings()
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ATinyTankCharacter::DetachComponent()
{
    SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    Camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    BaseMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    TurretMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    ProjectileSpawnPoint->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

void ATinyTankCharacter::ShowDeathEffects()
{
    if (DeathParticles && DeathSound && DeathCameraShakeClass && GetWorld()->GetFirstPlayerController())
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());

        UGameplayStatics::PlaySoundAtLocation
        (
            this,
            DeathSound,
            GetActorLocation()
        );

        GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DeathCameraShakeClass);
    }
}

void ATinyTankCharacter::CleanPlayerControllersData()
{
    if (TObjectPtr<APC_TinyTanks> PC_TinyTank = Cast<APC_TinyTanks>(GetController()))
    {
        PC_TinyTank->HandleDestructionOfTheCharacter();
    }
}