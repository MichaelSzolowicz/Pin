#include "Player/PinballMoveComponent.h"

#include "DrawDebugHelpers.h"

void UPinballMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	InverseMass = 1 / Mass;
	Velocity = FVector::Zero();
}


void UPinballMoveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePhysicsWithImpulse(DeltaTime);

	/*TESTONLY*/
	AActor* Owner = Cast<AActor>(GetOwner());
	FVector LogVelocity = Owner->GetVelocity();
	//UE_LOG(LogTemp, Warning, TEXT("Owner: %s velocity: %s"), *Owner->GetName(), *LogVelocity.ToString());
	/*ENDTEST*/
}

/*
void UPinballMoveComponent::UpdatePhysics(float DeltaTime)
{
	CalcGravity();

	// Delta position
	FVector d = Velocity * DeltaTime;
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	FVector N = FVector::Zero();

	// x move
	FHitResult OutHit;
	SafeMoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Red, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{	
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dx, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// y move
	SafeMoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Green, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dy, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// z move
	SafeMoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Blue, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dz, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	N.Normalize();
	N *= FVector::DotProduct((-Velocity / DeltaTime), N);
	AccumulatedForce += N;

	// Acting forces (ignore mass for now, force acts as acceleration)
	Velocity += AccumulatedForce * DeltaTime;

	AccumulatedForce = FVector::Zero();

	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Velocity.Size());
}
*/

void UPinballMoveComponent::UpdatePhysicsWithImpulse(float DeltaTime)
{
	CalcGravity();

	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.DeltaTime = DeltaTime;
	Move.EndVelocity = Velocity;
	

	APawn* Pawn = (APawn*)GetOwner();

	if (Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())
	{
		PerformMove(Move);
	}
	
	if (GetNetMode() == NM_Client)
	{
		ServerPerformMove(Move);	
	}
	AccumulatedForce = FVector::Zero();

}

void UPinballMoveComponent::ResolveCollision(FHitResult Hit)
{
	// Assuming other actor is static, I will need to find a different way to reliably get velocity from all types of actor.
	FVector rv = -Velocity;

	double velAlongNormal = FVector::DotProduct(rv, Hit.Normal);

	if (velAlongNormal < 0) return;

	float e = FMath::Min(restitution, .0f);	/** Need a reliable way to get the restitution for other objects. **/

	// actual impulse.
	float J = -(1 + e) * velAlongNormal;
	J /= InverseMass + 0;	/* Need a reliable way to get the inverse mass for other objects. */

	// Apply impulse
	FVector Impulse = J * Hit.Normal;
	Velocity -= InverseMass * Impulse;
}


void UPinballMoveComponent::CalcGravity()
{
	// Ignore mass (for the time being)
	AccumulatedForce += FVector(0, 0, GetWorld()->GetGravityZ());
}

void UPinballMoveComponent::PerformMove(FMove Move)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s Perform Move"), GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Velocity.ToString());

	// Delta position
	FVector d = Velocity * Move.DeltaTime;
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	// Update our position
	// Moving each axis indepently helps ensure walls apply an opposing force, canceling out our velcotiy and prevent wall "sticking"
	FHitResult Hit;
	SafeMoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dx, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Y move
	SafeMoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dy, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Z move
	SafeMoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dz, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());

	Velocity += Move.Force * Move.DeltaTime;

	AccumulatedForce = FVector::Zero();
}

void UPinballMoveComponent::ServerPerformMove_Implementation(FMove Move)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s Perform Move"), GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Move.Force.ToString());


	// Delta position
	FVector d = Velocity * Move.DeltaTime;
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	// Update our position
	// Moving each axis indepently helps ensure walls apply an opposing force, canceling out our velcotiy and prevent wall "sticking"
	FHitResult Hit;
	SafeMoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dx, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Y move
	SafeMoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dy, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Z move
	SafeMoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dz, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());

	Velocity += Move.Force * Move.DeltaTime;

	AccumulatedForce = FVector::Zero();

	CheckCompletedMove(Move);
}

void UPinballMoveComponent::CheckCompletedMove(FMove Move)
{
	bool correction = false;
	FMove CorrectedMove = FMove();

	UE_LOG(LogTemp, Warning, TEXT("Server / Client diff: %f"), FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()));
	if (FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()) > 1000.0)
	{
		correction = true;
		CorrectedMove.EndPosition = UpdatedComponent->GetComponentLocation();
	}

	if (correction)
	{
		ClientCorrection(CorrectedMove);
	}
}

void UPinballMoveComponent::ClientCorrection_Implementation(FMove Move)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client Correction"));
	UpdatedComponent->SetWorldLocation(Move.EndPosition);
}

void UPinballMoveComponent::AddForce(FVector Force)
{
	AccumulatedForce += Force;
}

float UPinballMoveComponent::AngleBetweenVectors(FVector v1, FVector v2)
{
	float Dot = FVector::DotProduct(v1, v2);
	float Mag = v1.Size() * v2.Size();
	return PI - (Dot / Mag);
}
