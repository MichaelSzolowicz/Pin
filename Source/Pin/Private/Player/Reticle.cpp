#include "Player/Reticle.h"


/*
* Offsets the reticle by provided input, using reticle sensitivity properties.
* @param Input
* @returns The reticle's new world locataion.
*/
FVector UReticle::AddInput(FVector2D Input)
{
	Offset += FVector(Input, 0.f);

	SetRelativeLocation(Offset);

	ClampPos();
	if (bConstrainToPlaneNormal) ConstrainToPlaneNormal();

	SetWorldRotation(GetRelativeLocation().Rotation());

	return GetComponentLocation();
}

void UReticle::ClampPos()
{
	FVector Clamped = Offset;
	if (Clamped.Size() > MaxRadius) {
		Clamped.Normalize();
		Clamped *= MaxRadius;
		Offset = Clamped;
	}
	else if (Clamped.Size() < MinRadius) {
		Clamped.Normalize();
		Clamped *= MinRadius;
		Offset = -Clamped;
	}
}

void UReticle::ConstrainToPlaneNormal()
{
	FVector Pos = Offset;
	float Size = Pos.Size();
	Pos.Normalize();

	// Calculate component of pos othogonal to the plane normal
	// defined as ((pos . n) / ||n||^2) * n
	// Subtract the orthogonal component from pos
	Pos = Pos - Pos.Dot(PlaneNormal) * PlaneNormal;

	// Update pos
	SetRelativeLocation(Pos * Size);
}

void UReticle::SetPlaneNormal(FVector Normal)
{
	PlaneNormal = Normal;

	if (bConstrainToPlaneNormal) ConstrainToPlaneNormal();
	SetWorldRotation(GetRelativeLocation().Rotation());
}
