#include "Player/PinballPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Player/DefaultPlayerInputConfig.h"

#include "Components/CapsuleComponent.h"
#include "Player/NetworkedPhysics.h"
#include "Player/Reticle.h"
#include "Projectiles/StickyProjectile.h"


APinballPlayer::APinballPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	NetworkPhysics = CreateDefaultSubobject<UNetworkedPhysics>(TEXT("NetworkPhysics"));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	RotationRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotationRoot"));
	RotationRoot->SetupAttachment(RootComponent);

	Reticle = CreateDefaultSubobject<UReticle>(TEXT("SpawnAt"));
	Reticle->SetupAttachment(RootComponent);
}

void APinballPlayer::BeginPlay()
{
	Super::BeginPlay();

	NetworkPhysics->OnServerReceiveMove.BindUObject(this, &APinballPlayer::AddGrappleForce);

	NetworkPhysics->SetUpdatedRotationComponent(RotationRoot);
}


void APinballPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	// Clear out existing mapping, and add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComp->BindAction(DefaultInputActions->InputPush, ETriggerEvent::Triggered, this, &APinballPlayer::Push);

	EnhancedInputComp->BindAction(DefaultInputActions->FireGrapple, ETriggerEvent::Started, this, &APinballPlayer::FireGrapple);

	EnhancedInputComp->BindAction(DefaultInputActions->SwivelReticle, ETriggerEvent::Triggered, this, &APinballPlayer::SwivelReticle);

}

void APinballPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetNetMode() == ENetMode::NM_Client) {
		AddGrappleForce();
	}
}

void APinballPlayer::Push(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Input push: %s"), *Input.ToString());

	NetworkPhysics->SetInput(Input);
}

void APinballPlayer::AddGrappleForce()
{
	if (GrappleProjectileComponent) {
		if (GrappleProjectileComponent->AttachedTo) {
			FVector Direction = GrappleProjectileComponent->GetOwner()->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			NetworkPhysics->AddForce(Direction * GrappleStrength);
		}
	}
}

void APinballPlayer::FireGrapple()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Grapple"));

	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, Reticle->GetComponentTransform());
	GrappleProjectileComponent = NewProj->GetComponentByClass<UStickyProjectile>();

	if (GetNetMode() == ENetMode::NM_Client) {
		ServerFireGrapple(GetWorld()->TimeSeconds);
	}
}


void APinballPlayer::ServerFireGrapple_Implementation(float Time)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerFireGrapple"));

	FMove SimulatedMove = FMove();
	SimulatedMove.Time = Time;
	NetworkPhysics->EstimateMoveFromBuffer(SimulatedMove);

	SimulatedMove.EndPosition += SimulatedMove.LookAt;

	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, SimulatedMove.EndPosition, SimulatedMove.LookAt.Rotation());
	GrappleProjectileComponent = NewProj->GetComponentByClass<UStickyProjectile>();
}


void APinballPlayer::SwivelReticle(const FInputActionValue& Value)
{
	FVector2D Offset = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Swivel offset: %s"), *Offset.ToString());

	if (Offset.Size() != 0.f) {
		Reticle->AddInput(Offset);
		NetworkPhysics->SetLookAtRotation(Reticle->GetRelativeLocation());
	}
}

