#include "Player/PinPlayerPawn.h"

APinPlayerPawn::APinPlayerPawn(const FObjectInitializer & ObjectInitializer)
{
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicsBody"));
	StaticMesh->SetupAttachment(RootComponent);
}
