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
	AddWorldOffset(FVector(Input, 0.f));
	SetWorldRotation(GetRelativeLocation().Rotation());

	ClampPos();

	return GetComponentLocation();
}

void UReticle::ClampPos()
{
	FVector Clamped = GetRelativeLocation();
	if (Clamped.Size() > MaxRadius) {
		Clamped.Normalize();
		Clamped *= MaxRadius;
		SetRelativeLocation(Clamped);
	}
}

