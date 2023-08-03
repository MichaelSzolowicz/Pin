#include "Player/Reticle.h"

// Sets default values for this component's properties
UReticle::UReticle()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UReticle::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UReticle::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/*
* Offsets the reticle by provided input, using reticle sensitivity properties.
* @param Input
* @returns The reticle's new world locataion.
*/
FVector UReticle::AddInput(FVector2D Input)
{
	FVector Input3D = FVector(Input.X, Input.Y, 0.f);
	AddWorldOffset(Input3D * Sensitivity);

	return GetComponentLocation();
}

