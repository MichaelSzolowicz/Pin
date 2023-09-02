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
	PrimaryActorTick.bCanEverTick = false;

	NetworkPhysics = CreateDefaultSubobject<UNetworkedPhysics>(TEXT("NetworkPhysics"));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	RotationRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotationRoot"));
	RotationRoot->SetupAttachment(RootComponent);

	Reticle = CreateDefaultSubobject<UReticle>(TEXT("SpawnAt"));
	Reticle->SetupAttachment(RootComponent);

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

void APinballPlayer::Push(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Input push: %s"), *Input.ToString());

	NetworkPhysics->SetInput(Input);
}

void APinballPlayer::FireGrapple()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Grapple"));

	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, Reticle->GetComponentTransform());
	GrappleProjectileComponent = NewProj->GetComponentByClass<UStickyProjectile>();

	if (GrappleProjectileComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Valid grapple projectile"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Grapple projectile"));
	}
}

void APinballPlayer::SwivelReticle(const FInputActionValue& Value)
{
	FVector2D Offset = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Swivel offset: %s"), *Offset.ToString());

	Reticle->AddInput(Offset);
}

