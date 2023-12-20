#include "Ai/EnemyMovement.h"


void UEnemyMovement::Move(float DeltaTime)
{
	//Input
	FVector Input = ConsumeInputDirection();
	if (Input.Size() > 0) {
		FVector Dv = Input * Acceleration * DeltaTime;
		FVector Target = MovementVelocity + Dv;

		// Take friction out
		MovementVelocity -= MovementVelocity - MovementVelocity * FMath::Clamp(1.0f - Turning, 0, 1);

		// Add new input
		FVector NewVelocity = MovementVelocity + Dv;

		// Compensate for lost accel or speed, but use new direction
		NewVelocity = NewVelocity.GetSafeNormal() * Target.Size();

		if (NewVelocity.Size() > MaxSpeed) {
			// Let input counteract velocity over max speed.
			float Dot = Dv.Dot(VelocityOverMax.GetSafeNormal());
			Dot = FMath::Clamp(Dot, -VelocityOverMax.Size(), VelocityOverMax.Size());
			if (Dot < 0) {
				VelocityOverMax += Dot * VelocityOverMax.GetSafeNormal();
				Dv += Dot * VelocityOverMax.GetSafeNormal();
			}
			NewVelocity = MovementVelocity + Dv;

			NewVelocity = NewVelocity.GetSafeNormal() * MaxSpeed;
		}

		MovementVelocity = NewVelocity;
	}

	// Braking
	if(Input.Size() <= 0 || (MovementVelocity + VelocityOverMax).Size() > MaxSpeed){
		FVector ActualVelocity = MovementVelocity + VelocityOverMax;

		FVector Delta;

		if ((MovementVelocity + VelocityOverMax).Size() > MaxSpeed) {
			Delta = ActualVelocity - (ActualVelocity * FMath::Clamp(1.0f - Braking * BrakingOverMaxSpeed, 0.0f, 1.0f));
		}
		else {
			Delta = ActualVelocity - (ActualVelocity * FMath::Clamp(1.0f - Braking, 0.0f, 1.0f));
		}

		VelocityOverMax -= FMath::Clamp(Delta.Dot(VelocityOverMax.GetSafeNormal()), 0, VelocityOverMax.Size()) * VelocityOverMax.GetSafeNormal();
		Delta -= FMath::Clamp(Delta.Dot(VelocityOverMax.GetSafeNormal()), 0, VelocityOverMax.Size()) * VelocityOverMax.GetSafeNormal();
		MovementVelocity -= Delta;
	}

	FVector DeltaPos = (MovementVelocity + VelocityOverMax) * DeltaTime;

	DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + VelocityOverMax, FColor::Blue, false, .166f);

	// Velocity is no longer "over max," so get rid of it. 
	if ((MovementVelocity + VelocityOverMax).Size() <= MaxSpeed) {
		MovementVelocity += VelocityOverMax;
		VelocityOverMax = FVector::Zero();
	}

	FHitResult Hit;

	SafeMoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, Hit);
	if (Hit.bBlockingHit) {
		float VelAlongNormal = MovementVelocity.Dot(Hit.Normal);
		MovementVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);

		SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
	}
}

void UEnemyMovement::AddInputDirection(FVector Direction)
{
	Direction.Normalize();
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
	MovementVelocity += Force * GetWorld()->DeltaTimeSeconds;
	if (MovementVelocity.Size() > MaxSpeed) {
		FVector T = MovementVelocity.GetSafeNormal() * (MovementVelocity.Size() - MaxSpeed);
		MovementVelocity -= T;
		VelocityOverMax += T;
	}
}
