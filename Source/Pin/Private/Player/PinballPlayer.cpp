#include "Player/PinballPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Player/DefaultPlayerInputConfig.h"

#include "Components/CapsuleComponent.h"
#include "Player/NetworkedPhysics.h"
#include "Player/Reticle.h"
#include "Projectiles/StickyProjectile.h"
#include "PawnUtilities.h"


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
	EnhancedInputComp->BindAction(DefaultInputActions->FireGrapple, ETriggerEvent::Completed, this, &APinballPlayer::ReleaseGrapple);

	EnhancedInputComp->BindAction(DefaultInputActions->FireWeapon, ETriggerEvent::Started, this, &APinballPlayer::FireWeapon);
	EnhancedInputComp->BindAction(DefaultInputActions->FireWeapon, ETriggerEvent::Completed, this, &APinballPlayer::ReleaseWeapon);

	EnhancedInputComp->BindAction(DefaultInputActions->SwivelReticle, ETriggerEvent::Triggered, this, &APinballPlayer::SwivelReticle);

}

void APinballPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OrientToFloor();
}

void APinballPlayer::OrientToFloor()
{
	FQuat DeltaRotation = UPawnUtilities::RotateToFloor(RotationRoot);
	RotationRoot->AddWorldRotation(DeltaRotation);
}


/**
* Apply force directly to player via network physics component.
*/
void APinballPlayer::Push(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Input push: %s"), *Input.ToString());

	NetworkPhysics->SetInput(Input);
}

/**
* Add force directed towards the grapple projectile, if it is attached.
* If a local player controller, runs every frame until grapple projectile is invalid.
*/
void APinballPlayer::AddGrappleForce()
{
	UE_LOG(LogTemp, Warning, TEXT("Add Grapple Force"));

	if (IsValid(GrappleProjectileComponent)) {
		if (Controller->IsLocalPlayerController()) {
			GetWorldTimerManager().SetTimerForNextTick(this, &APinballPlayer::AddGrappleForce);
		}

		if (IsValid(GrappleProjectileComponent->GetAttachedTo())) {
			FVector Direction = GrappleProjectileComponent->GetOwner()->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			NetworkPhysics->AddForce(Direction * GrappleStrength);
		}
	}
}


/**
* Launch instance of Grapple Projectile Class.
*/
void APinballPlayer::FireGrapple()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Grapple"));

	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, Reticle->GetComponentTransform());
	GrappleProjectileComponent = NewProj->GetComponentByClass<UStickyProjectile>();

	GrappleProjectileComponent->OnAttached.Unbind();
	GrappleProjectileComponent->OnAttached.BindUObject(this, &APinballPlayer::AddGrappleForce);

	if (GetNetMode() == ENetMode::NM_Client) {
		ServerFireGrapple(GetWorld()->TimeSeconds, Reticle->GetRelativeLocation());
	}
}


/**
* Destroy grapple projectile on the client, send rpc to server.
*/
void APinballPlayer::ReleaseGrapple()
{
	if (GrappleProjectileComponent && GrappleProjectileComponent->GetOwner()) {
		GrappleProjectileComponent->GetOwner()->Destroy();
		GrappleProjectileComponent->DestroyComponent();

		ServerReleaseGrapple();
	}
}


/**
* Launch Grapple Projectile on server. 
* @param Time - Spawns from the position in network physics' move buffer with nearest timestamp.
* @param LookAt - Combined forward vector / offset from root.
*/
void APinballPlayer::ServerFireGrapple_Implementation(float Time, FVector LookAt)
{
	UE_LOG(LogTemp, Warning, TEXT("ServerFireGrapple"));

	FMove SimulatedMove = FMove();
	SimulatedMove.Time = Time;
	NetworkPhysics->EstimateMoveFromBuffer(SimulatedMove);

	// Should clamp look at if it is greater than reticle radius.
	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, SimulatedMove.EndPosition + LookAt, LookAt.Rotation());
	GrappleProjectileComponent = NewProj->GetComponentByClass<UStickyProjectile>();

	GrappleProjectileComponent->UpdatePhysics(NetworkPhysics->MoveBufferLast().Time - SimulatedMove.Time);
}


/**
* Destroy grapple projectile on server.
*/
void APinballPlayer::ServerReleaseGrapple_Implementation()
{
	if (GrappleProjectileComponent) {
		GrappleProjectileComponent->GetOwner()->Destroy();
		GrappleProjectileComponent->DestroyComponent();
	}
}


/**
* Launch instance Default Weapon Projectile. 
*/
void APinballPlayer::FireWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));

	GetWorld()->SpawnActor<AActor>(DefaultWeaponProjectile, Reticle->GetComponentTransform());

	if (GetNetMode() == NM_Client) {
		ServerFireWeapon(GetWorld()->TimeSeconds, Reticle->GetRelativeLocation());
	}
}


/**
* Called when the player releases the fire weapon button.
*/
void APinballPlayer::ReleaseWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Release Weapon"));
}


/**
* Launch Default Weapon Projectile on server.
* @param Time - Spawns from the position in network physics' move buffer with nearest timestamp.
* @param LookAt - Combined forward vector / offset from root.
*/
void APinballPlayer::ServerFireWeapon_Implementation(float Time, FVector LookAt)
{
	UE_LOG(LogTemp, Warning, TEXT("Server Fire Weapon"));

	FMove SimulatedMove = FMove();
	SimulatedMove.Time = Time;
	NetworkPhysics->EstimateMoveFromBuffer(SimulatedMove);

	// Should clamp look at if it is greater than reticle radius.
	AActor* NewProj = GetWorld()->SpawnActor<AActor>(DefaultWeaponProjectile, SimulatedMove.EndPosition + LookAt, LookAt.Rotation());
	USimpleProjectile* SimpleProjectile = NewProj->GetComponentByClass<USimpleProjectile>();

	SimpleProjectile->UpdatePhysics(NetworkPhysics->MoveBufferLast().Time - SimulatedMove.Time);
}


/**
* Can be called to perform actions on realse weapon on server.
*/
void APinballPlayer::ServerReleaseWeapon_Implementation()
{
}


/**
* Update reticle offset using mouse delta.
*/
void APinballPlayer::SwivelReticle(const FInputActionValue& Value)
{
	FVector2D Offset = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Swivel offset: %s"), *Offset.ToString());

	if (Offset.Size() != 0.f) {
		Reticle->AddInput(Offset);
		NetworkPhysics->SetLookAtRotation(Reticle->GetRelativeLocation());
	}
}

