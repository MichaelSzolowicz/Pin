#include "Ai/EnemyMovement.h"

#include "PawnUtilities.h"


void UEnemyMovement::Move(float DeltaTime)
{
	FVector Input = ConsumeInputDirection();

	//Input
	ApplyInput(Input, DeltaTime);

	// Braking
	if(Input.Size() <= 0 || ActualVelocity().Size() > MaxSpeed + TOLERANCE) {
		ApplyBraking(DeltaTime);
	}

	FVector DeltaPos = ActualVelocity() * DeltaTime;
	FHitResult Hit;

	// Physically move the actor
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

	DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + ExternalVelocity, FColor::Blue, false, 0.08f);
}

void UEnemyMovement::MoveDown(float DeltaTime)
{
	bConstrainToPlane = UPawnUtilities::RotateToFloor(UpdatedComponent, 52.1f);
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

void UEnemyMovement::ApplyInput(FVector Input, float DeltaTime)
{
	if (Input.Size() <= 0) return;

	FVector Dv = Input * DeltaTime * Acceleration;

	float ActualTurning = ActualVelocity().Size() > MaxSpeed + TOLERANCE ? TurningWhileSpeeding : Turning;
	ControlVelocity = (Dv + (ControlVelocity - ControlVelocity * FMath::Clamp(ActualTurning, 0, 1))).GetSafeNormal() * ControlVelocity.Size();

	if ((ControlVelocity + Dv).Size() > MaxSpeed + TOLERANCE) {
		FVector ExcessAccel = ((ControlVelocity + Dv).Size() - MaxSpeed) * Dv.GetSafeNormal();
		Dv -= ExcessAccel;
		ControlVelocity = (ControlVelocity + Dv).GetSafeNormal() * MaxSpeed;

		// Let input counteract excess velocity.
		// Horribly ineffecient but gets the expressiveness I want across. Should optimize later.
		if (ExternalVelocity.X * ExcessAccel.X < 0) {
			if (FMath::Abs(ExcessAccel.X) > FMath::Abs(ExternalVelocity.X)) {
				ExternalVelocity.X = 0;
			}
			else {
				ExternalVelocity.X += ExcessAccel.X;
			}
		}
		if (ExternalVelocity.Y * ExcessAccel.Y < 0) {
			if (FMath::Abs(ExcessAccel.Y) > FMath::Abs(ExternalVelocity.Y)) {
				ExternalVelocity.Y = 0;
			}
			else {
				ExternalVelocity.Y += ExcessAccel.Y;
			}
		}
		if (ExternalVelocity.Z * ExcessAccel.Z < 0) {
			if (FMath::Abs(ExcessAccel.Z) > FMath::Abs(ExternalVelocity.Z)) {
				ExternalVelocity.Z = 0;
			}
			else {
				ExternalVelocity.Z += ExcessAccel.Z;
			}
		}
	}
	else {
		ControlVelocity += Dv;
	}
}

void UEnemyMovement::ApplyBraking(float DeltaTime)
{
	
	float ActualBraking = ActualVelocity().Size() > MaxSpeed + TOLERANCE ? BrakingWhileSpeeding : Braking;

	ExternalVelocity -= ActualVelocity() - (ActualVelocity() * FMath::Clamp(1.0f - ActualBraking, 0.0f, 1.0f));
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

void UEnemyMovement::AddForce(FVector Force)
{
	FTimerDelegate AddForceDelayed;
	AddForceDelayed.BindUFunction(this, FName("AddForceInternal"), Force);
	GetWorld()->GetTimerManager().SetTimerForNextTick(AddForceDelayed);
}

void UEnemyMovement::AddForceInternal(FVector Force)
{
	ExternalVelocity += Force * GetWorld()->DeltaTimeSeconds;
}
