#include "Ai/EnemyMovement.h"


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
	MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);
	if (Hit.bBlockingHit) {
		// Apply normal impulse to control and excess velocities.
		float VelAlongNormal = ControlVelocity.Dot(Hit.Normal);
		ControlVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);
		VelAlongNormal = ExcessVelocity.Dot(Hit.Normal);
		ExcessVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);

		SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
	}

	/* TESTONLY */
	DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + (ExcessVelocity), FColor::Blue, false, .166f);
	/* ENDTEST */
}

void UEnemyMovement::ApplyInput(FVector Input, float DeltaTime)
{
	if (Input.Size() <= 0) return;

	FVector Dv = Input * Acceleration * DeltaTime;

	// Store target speed for steering
	float TargetSpeed = (ControlVelocity + Dv).Size();

	// Take friction out
	float ActualTurning = ActualVelocity().Size() > MaxSpeed + TOLERANCE ? TurningWhileSpeeding : Turning;
	ControlVelocity -= ControlVelocity - ControlVelocity * FMath::Clamp(1.0f - ActualTurning, 0, 1);

	// Add new input
	FVector NewVelocity = ControlVelocity + Dv;

	// Compensate for lost accel or speed, but use new direction
	NewVelocity = NewVelocity.GetSafeNormal() * TargetSpeed;

	if (NewVelocity.Size() > MaxSpeed + TOLERANCE) {
		// Let input counteract excess velocity.
		float Dot = Dv.Dot(ExcessVelocity.GetSafeNormal());
		Dot = FMath::Clamp(Dot, -ExcessVelocity.Size(), ExcessVelocity.Size());
		if (Dot < 0) {
			FVector CounterExcessVelocity = Dot * ExcessVelocity.GetSafeNormal();
			ExcessVelocity += CounterExcessVelocity;
			NewVelocity -= CounterExcessVelocity;
		}

		NewVelocity = NewVelocity.GetSafeNormal() * MaxSpeed;
	}

	ControlVelocity = NewVelocity;
}

void UEnemyMovement::ApplyBraking(float DeltaTime)
{
	float ActualBraking = ActualVelocity().Size() > MaxSpeed + TOLERANCE ? BrakingWhileSpeeding : Braking;

	// Apply braking as a fraction of the actual velocity.
	FVector Delta = ActualVelocity() - (ActualVelocity() * FMath::Clamp(1.0f - ActualBraking, 0.0f, 1.0f));

	// Apply braking along excess velocity to excess velocity
	FVector BrakingExcessVelocity = FMath::Clamp(Delta.Dot(ExcessVelocity.GetSafeNormal()), 0, ExcessVelocity.Size()) * ExcessVelocity.GetSafeNormal();

	ExcessVelocity -= BrakingExcessVelocity;
	Delta -= BrakingExcessVelocity;
	ControlVelocity -= Delta;

	// Velocity is no longer "over max," so get rid of it.
	if (ActualVelocity().Size() <= MaxSpeed + TOLERANCE) {
		ControlVelocity += ExcessVelocity;
		ExcessVelocity = FVector::Zero();
	}
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
	ControlVelocity += Force * GetWorld()->DeltaTimeSeconds;

	if (ControlVelocity.Size() > MaxSpeed + TOLERANCE) {
		FVector T = ControlVelocity.GetSafeNormal() * (ControlVelocity.Size() - MaxSpeed);
		ControlVelocity -= T;
		ExcessVelocity += T;
	}
}
