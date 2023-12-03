#include "Ai/EnemyMovement.h"

void UEnemyMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

void UEnemyMovement::Move(float DeltaTime)
{
	FVector Input = ConsumeInputDirection();
	if (Input.Size() > 0) {
		FVector Dv = Input * Acceleration * DeltaTime;
		FVector NewVelocity = MovementVelocity + Dv;

		// If exceeding max speed, prevent acceleration along current trajectory, but allow change in direction..
		if (NewVelocity.Size() > MaxSpeed) {
			float AccelAlongVelocity = Dv.Dot(MovementVelocity.GetSafeNormal());
			if (AccelAlongVelocity > 0) {
				NewVelocity -= MovementVelocity.GetSafeNormal() * AccelAlongVelocity;
			}
		}
		
		DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
			UpdatedComponent->GetComponentLocation() + (NewVelocity - MovementVelocity) * 200, FColor::Blue, false, .166f);

		MovementVelocity = NewVelocity;
	}
	else if(MovementVelocity.Size() <= MaxSpeed){
		MovementVelocity -= MovementVelocity - (MovementVelocity * FMath::Clamp(1.0f - Braking, 0.0f, 1.0f));
	}

	// Always apply counter acceleration when exceeding max speed.
	if (MovementVelocity.Size() >= MaxSpeed) {
		FVector Diff = MovementVelocity - MovementVelocity.GetSafeNormal() * MaxSpeed;

		MovementVelocity -= Diff - (Diff * FMath::Clamp(1.0f - BrakingOverMaxSpeed / Diff.Size(), 0.0f, 1.0f));

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Diff: %f"), Diff.Size()));

		if (Diff.Size() <= Acceleration * DeltaTime) {
			MovementVelocity = MovementVelocity.GetSafeNormal() * MaxSpeed;
		}
	}

	FVector DeltaPos = MovementVelocity * DeltaTime;

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
}
