#include "Player/PinPlayerPawn.h"

#include "Player/PhysicsBodyComponent.h"

APinPlayerPawn::APinPlayerPawn(const FObjectInitializer & ObjectInitializer)
{
	

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	PhysicsBody = CreateDefaultSubobject<UPhysicsBodyComponent>(TEXT("PhysicsBody"));
	PhysicsBody->SetupAttachment(RootComponent);
}

// Called to bind functionality to input
void APinPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis(TEXT("InputX"));
	InputComponent->BindAxis(TEXT("InputY"));
}

void APinPlayerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	
}

void APinPlayerPawn::RollBody()
{
	if (PhysicsBody) 
	{
		FVector Torque;
		Torque.X = GetInputAxisValue("InputX");
		Torque.Y = GetInputAxisValue("InputY");

		PhysicsBody->AddTorqueInDegrees(Torque * FMath::Pow(ScaleInputTorque, 2.0f));
	}
}
