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
	Input.Normalize();	// Should change steering comp do it doesn't multiply input by a speed.

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Enemy move: %s"), *Input.ToString()));

	Movement->AddInputDirection(Input);
	Movement->Move(DeltaTime);
}

void ASteeringEnemy::Move()
{

}
