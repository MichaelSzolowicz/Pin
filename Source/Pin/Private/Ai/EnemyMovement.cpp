#include "Ai/EnemyMovement.h"

#include "PawnUtilities.h"


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
	MoveDown(DeltaTime);
	MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);
	if (Hit.bBlockingHit) {
		float VelAlongNormal = ComponentVelocity.Dot(Hit.Normal);
		ComponentVelocity += Hit.Normal * FMath::Abs(VelAlongNormal);

		SlideAlongSurface(DeltaPos, 1.0f - Hit.Time, Hit.Normal, Hit);
	}

	DrawDebugLine(GetWorld(), UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + (ComponentVelocity - InputVelocity), FColor::Blue, false, 0.08f);
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
	if (ComponentVelocity.Size() <= MaxSpeed + TOLERANCE) InputVelocity = ComponentVelocity;

	ComponentVelocity -= InputVelocity;
	FVector Dv = Input * DeltaTime * Acceleration;

	float ActualTurning = ComponentVelocity.Size() > MaxSpeed + TOLERANCE ? TurningWhileSpeeding : Turning;
	InputVelocity = (Dv + (InputVelocity - InputVelocity * FMath::Clamp(ActualTurning, 0, 1))).GetSafeNormal() * InputVelocity.Size();

	if ((InputVelocity + Dv).Size() > MaxSpeed + TOLERANCE) {
		FVector ExcessAccel = ((InputVelocity + Dv).Size() - MaxSpeed) * Dv.GetSafeNormal();
		Dv -= ExcessAccel;
		InputVelocity = (InputVelocity + Dv).GetSafeNormal() * MaxSpeed;
		ComponentVelocity += InputVelocity;

		// Let input counteract excess velocity.
		if (ComponentVelocity.Size() > MaxSpeed + TOLERANCE) {
			FVector ExcessVelocity = ComponentVelocity - InputVelocity;
			
			// Horribly ineffecient but gets the expressiveness I want across. Should optimize later.
			if (ExcessVelocity.X * ExcessAccel.X < 0) {
				if (FMath::Abs(ExcessAccel.X) > FMath::Abs(ExcessVelocity.X)) {
					ExcessVelocity.X = 0;
				}
				else {
					ExcessVelocity.X += ExcessAccel.X;
				}
			}
			if (ExcessVelocity.Y * ExcessAccel.Y < 0) {
				if (FMath::Abs(ExcessAccel.Y) > FMath::Abs(ExcessVelocity.Y)) {
					ExcessVelocity.Y = 0;
				}
				else {
					ExcessVelocity.Y += ExcessAccel.Y;
				}
			}
			if (ExcessVelocity.Z * ExcessAccel.Z < 0) {
				if (FMath::Abs(ExcessAccel.Z) > FMath::Abs(ExcessVelocity.Z)) {
					ExcessVelocity.Z = 0;
				}
				else {
					ExcessVelocity.Z += ExcessAccel.Z;
				}
			}

			ComponentVelocity = InputVelocity + ExcessVelocity;
		}
	}
	else {
		InputVelocity += Dv;
		ComponentVelocity += InputVelocity;
	}
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
