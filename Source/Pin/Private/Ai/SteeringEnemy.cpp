#include "Ai/SteeringEnemy.h"

#include "Ai/AiSteering.h"
#include "Ai/EnemyMovement.h"


ASteeringEnemy::ASteeringEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Movement = CreateDefaultSubobject<UEnemyMovement>(TEXT("Movement"));

	Steering = CreateDefaultSubobject<UAiSteering>(TEXT("Steering"));
}

void ASteeringEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASteeringEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Input = Steering->GetInput();
	Input.Normalize();
	//Input.Z = 0.0f;

	// Try to move against velocity if we are speeding
	if (Movement->GetComponentVelocity().Size() > Movement->GetMaxSpeed() + 0.1f) {
		// * 1.05 so counter velocity input is always larger than base input.
		Input += -Movement->GetComponentVelocity().GetSafeNormal() * 1.05f;
		Input.Normalize();
	}



	Movement->AddInputDirection(Input);
	Movement->Move(DeltaTime);

	Input.Z = 0;
	DrawDebugLine(GetWorld(), GetActorLocation(),
		GetActorLocation() + Input * 200, FColor::Magenta, false, 1.0f);
}

void ASteeringEnemy::Move()
{

}
