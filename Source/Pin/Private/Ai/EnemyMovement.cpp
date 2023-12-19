#include "Ai/EnemyMovement.h"


void UEnemyMovement::Move(float DeltaTime)
{
	FVector Input = ConsumeInputDirection();
	if (Input.Size() > 0) {
		FVector Dv = Input * Acceleration * DeltaTime;
		FVector NewVelocity = MovementVelocity + Dv;

		if (NewVelocity.Size() > MaxSpeed) {
			// Let input counteract velocity over max speed.
			float Dot = Dv.Dot(VelocityOverMax.GetSafeNormal());
			if (Dot < 0) {
				VelocityOverMax -= Dot * Dv.GetSafeNormal();
				Dv -= Dot * Dv.GetSafeNormal();
			}
			NewVelocity = MovementVelocity + Dv;

			NewVelocity = NewVelocity.GetSafeNormal() * MaxSpeed;
		}

		DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
			UpdatedComponent->GetComponentLocation() + (NewVelocity - MovementVelocity) * 200, FColor::Blue, false, .166f);

		MovementVelocity = NewVelocity;
	}
	else if((MovementVelocity + VelocityOverMax).Size() <= MaxSpeed + .10f){
		MovementVelocity -= MovementVelocity - (MovementVelocity * FMath::Clamp(1.0f - Braking, 0.0f, 1.0f));
	}

	// Multiplying by (1 - BrakingOverMaxSpeed) allows braking to scale with speed, while still allowing a braking value of 1 to cause an instant stop.
	VelocityOverMax -= VelocityOverMax - (VelocityOverMax * FMath::Clamp(1.0f - BrakingOverMaxSpeed / VelocityOverMax.Size(), 0.0f, 1.0f) * FMath::Clamp(1.0f - BrakingOverMaxSpeed, 0.0f, 1.0f));
	
	FVector DeltaPos = (MovementVelocity + VelocityOverMax) * DeltaTime;

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
