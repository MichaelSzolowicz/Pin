#include "Ai/SteeringEnemy.h"

#include "Player/NetworkedPhysics.h"
#include "Ai/AiSteering.h"


ASteeringEnemy::ASteeringEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Physics = CreateDefaultSubobject<UNetworkedPhysics>(TEXT("Physics"));

	Steering = CreateDefaultSubobject<UAiSteering>(TEXT("Steering"));
}

void ASteeringEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASteeringEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Move();
}

void ASteeringEnemy::Move()
{
	FVector Input = Steering->GetInput();
	Input.Normalize();	// Should change steering comp do it doesn't multiply input by a speed.

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Enemy move: %s"), *Input.ToString()));

	Physics->AddForce(100.0f * BaseAcceleration * Input);
}
