#include "Ai/EnemyMovement.h"


void UEnemyMovement::Move(float DeltaTime)
{
	FVector Input = ConsumeInputDirection();

	//Input
	ApplyInput(Input, DeltaTime);

	// Braking
	if(Input.Size() <= 0 || ComponentVelocity.Size() > MaxSpeed + TOLERANCE) {
		ApplyBraking(DeltaTime);
	}

	FVector DeltaPos = ComponentVelocity * DeltaTime;
	FHitResult Hit;

	// Physically move the actor
	MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);
	if (Hit.bBlockingHit) {
		float VelAlongNormal = ComponentVelocity.Dot(Hit.Normal);
		ComponentVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);

		SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
	}
}

void UEnemyMovement::ApplyInput(FVector Input, float DeltaTime)
{
	if (Input.Size() <= 0) {
		return;
	}
	if (ComponentVelocity.Size() < MaxSpeed + TOLERANCE) {
		InputVelocity = ComponentVelocity;
	}

	FVector Dv = Input * Acceleration * DeltaTime;
	FVector NonInputVelocity = ComponentVelocity - InputVelocity;

	// Store target speed for steering
	float TargetSpeed = (InputVelocity + Dv).Size();

	// Take friction out and add input
	float ActualTurning = ComponentVelocity.Size() > MaxSpeed + TOLERANCE ? TurningWhileSpeeding : Turning;
	FVector NewVelocity = InputVelocity - InputVelocity * FMath::Clamp(ActualTurning, 0, 1) + Dv;

	// Compensate for lost accel or speed, but use new direction
	NewVelocity = NewVelocity.GetSafeNormal() * TargetSpeed;
	Dv = NewVelocity - InputVelocity;

	if (NewVelocity.Size() > MaxSpeed + TOLERANCE) {
		// Let input counteract excess velocity.
		float Dot = Dv.Dot(NonInputVelocity.GetSafeNormal());
		Dot = FMath::Clamp(Dot, -NonInputVelocity.Size(), NonInputVelocity.Size());
		if (Dot < 0) {
			FVector CounterExcessVelocity = Dot * NonInputVelocity.GetSafeNormal();
			NonInputVelocity += CounterExcessVelocity;
			NewVelocity -= CounterExcessVelocity;
		}

		NewVelocity = NewVelocity.GetSafeNormal() * MaxSpeed;
	}

	InputVelocity = NewVelocity;
	ComponentVelocity = NonInputVelocity + InputVelocity;
}

void UEnemyMovement::ApplyBraking(float DeltaTime)
{
	float ActualBraking = ComponentVelocity.Size() > MaxSpeed + TOLERANCE ? BrakingWhileSpeeding : Braking;

	ComponentVelocity -= ComponentVelocity - (ComponentVelocity * FMath::Clamp(1.0f - ActualBraking, 0.0f, 1.0f));
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
	ComponentVelocity += Force * GetWorld()->DeltaTimeSeconds;
}
