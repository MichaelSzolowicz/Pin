#include "Ai/EnemyMovement.h"

#include "PawnUtilities.h"


void UEnemyMovement::Move(float DeltaTime)
{
	FVector Input = ConsumeInputDirection();

	//Input
	FVector Dv = ApplyInput(Input, DeltaTime);

	// Braking
	if (Dv.Size() <= 0 || ActualVelocity().Size() > MaxSpeed + TOLERANCE) {
		ApplyBraking(DeltaTime);
	}

	// Physically move the actor
	FVector DeltaPos = ActualVelocity() * DeltaTime;
	FHitResult Hit;

	MoveDown(DeltaTime);
	MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);
	if (Hit.bBlockingHit) {
		float VelAlongNormal = ActualVelocity().Dot(Hit.Normal);
		ExternalVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);

		SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
	}

	// Collapse velocities if we are under max speed, that is to say in control.
	if (ActualVelocity().Size() < MaxSpeed) {
		ControlVelocity = ActualVelocity();
		ExternalVelocity = FVector::Zero();
	}

	// Apply accumulated forces after movement so they do not interfere with calculations.
	ApplyAccumulatedImpulse();

	/*TESTONLY*/
	DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + ExternalVelocity, FColor::Blue, false, 0.08f);
	/*ENDTEST*/
}

void UEnemyMovement::MoveDown(float DeltaTime)
{
	float Depth = UpdatedComponent->Bounds.BoxExtent.Z;
	bConstrainToPlane = UPawnUtilities::RotateToFloor(UpdatedComponent, Depth + 3.0f);
	SetPlaneConstraintNormal(UpdatedComponent->GetUpVector());

	FVector DeltaPos = FVector::DownVector * FallSpeed * DeltaTime;
	FHitResult Hit;

	if (!bConstrainToPlane) {
		MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);

		if (Hit.bBlockingHit) {
			SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
		}
	}
}

FVector UEnemyMovement::ApplyInput(FVector Input, float DeltaTime)
{
	if (Input.Size() <= 0) return Input;

	FVector Dv = Input * DeltaTime * Acceleration;

	// Let input counteract excess velocity.
	// Horribly ineffecient but gets the expressiveness I want across. Could optimize later.
	if (ExternalVelocity.X * Dv.X < 0) {
		if (FMath::Abs(Dv.X) > FMath::Abs(ExternalVelocity.X)) {
			Dv.X += ExternalVelocity.X;
			ExternalVelocity.X = 0;
		}
		else {
			ExternalVelocity.X += Dv.X;
			Dv.X = 0;
		}
	}
	if (ExternalVelocity.Y * Dv.Y < 0) {
		if (FMath::Abs(Dv.Y) > FMath::Abs(ExternalVelocity.Y)) {
			Dv.Y += ExternalVelocity.Y;
			ExternalVelocity.Y = 0;
		}
		else {
			ExternalVelocity.Y += Dv.Y;
			Dv.Y = 0;
		}
	}
	if (ExternalVelocity.Z * Dv.Z < 0) {
		if (FMath::Abs(Dv.Z) > FMath::Abs(ExternalVelocity.Z)) {
			Dv.Z += ExternalVelocity.Z;
			ExternalVelocity.Z = 0;
		}
		else {
			ExternalVelocity.Z += Dv.Z;
			Dv.Z = 0;
		}
	}

	// Add remaining input to control velocity, but do not go over max speed.
	ControlVelocity += Dv;
	if (ControlVelocity.Size() > MaxSpeed + TOLERANCE) {
		ControlVelocity = ControlVelocity.GetSafeNormal() * MaxSpeed;
	}

	return Dv;
}

void UEnemyMovement::ApplyBraking(float DeltaTime)
{
	float ActualBraking = ActualVelocity().Size() > MaxSpeed + TOLERANCE ? BrakingWhileSpeeding : Braking;
	ExternalVelocity -= ActualVelocity() - (ActualVelocity() * FMath::Clamp(1.0f - ActualBraking * DeltaTime, 0.0f, 1.0f));
}

void UEnemyMovement::AddInputDirection(FVector Direction)
{
	InputDirection += Direction;
	InputDirection.Normalize();
}

FVector UEnemyMovement::ConsumeInputDirection()
{
	FVector Temp = InputDirection;
	InputDirection = FVector::Zero();
	return Temp;
}

void UEnemyMovement::AddImpulse(FVector Impulse)
{
	AccumulatedImpulse += Impulse;
}

void UEnemyMovement::ApplyAccumulatedImpulse()
{
	ExternalVelocity += AccumulatedImpulse;
	AccumulatedImpulse = FVector::Zero();
}
