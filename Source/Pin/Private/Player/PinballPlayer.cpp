#include "Player/PinballPlayer.h"


#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Player/DefaultPlayerInputConfig.h"

#include "Components/CapsuleComponent.h"
#include "Player/NetworkedPhysics.h"


APinballPlayer::APinballPlayer()
{
	PrimaryActorTick.bCanEverTick = false;

	NetworkPhysics = CreateDefaultSubobject<UNetworkedPhysics>(TEXT("NetworkPhysics"));

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetupAttachment(RootComponent);

	RotationRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RotationRoot"));
	RotationRoot->SetupAttachment(RootComponent);

	SpawnAt = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnAt"));
	SpawnAt->SetupAttachment(RootComponent);

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

}

void APinballPlayer::Push(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	UE_LOG(LogTemp, Warning, TEXT("Input push: %s"), *Input.ToString());

	NetworkPhysics->SetInput(Input.X, Input.Y);
}

