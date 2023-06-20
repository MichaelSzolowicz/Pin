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
	Move.EndVelocity = Velocity;
	Move.EndPosition = UpdatedComponent->GetComponentLocation();
	if (GetNetMode() == NM_Client)
	{
		ServerPerformMove(Move);
	}
}

void UPinballMoveComponent::ResolveCollision(FHitResult Hit)
{
	// Assuming other actor is static, later I will need to find a different way to reliably get velocity from all types of actor.
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

	EndPos += d;

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

bool UPinballMoveComponent::ServerPerformMove_Validate(FMove Move)
{
	// When declaring a RPC WithValidation, the _Validate function is automatically called and will disconnect the client if it returns false.
	// Only use this feature if you are sure the client is cheating.

	UE_LOG(LogTemp, Warning, TEXT("Server Validate"));
	return true;
}

bool UPinballMoveComponent::ServerValidateMove(FMove Move)
{
	// Will fill out this function later.
	// I need a better system for determining if a move is valid.
	// I think I should break player input and natural physical forces into seperate inputs.
	// This way the server can validate them seperatly, w/out running into issues like capping the max fall speed when we don't want to.

	// Although in this case we might only need to send over the input. Physical forces should be the same on the server, so we can calculate those
	// using the constants on the server and client provided delta time.
	return false;
}

void UPinballMoveComponent::CheckCompletedMove(FMove Move)
{
	// The server's most recent approved position, velocity, etc. is assigned to the components they belong to.
	// The result of the move sent by the client is stored in struct members prefixed by "End."
	bool correction = false;

	UE_LOG(LogTemp, Warning, TEXT("Server / Client distance: %f"), FVector::Distance(Move.EndPosition,UpdatedComponent->GetComponentLocation()));
	UE_LOG(LogTemp, Warning, TEXT("Client Move End Position: %f"), *Move.EndPosition.ToString());

	if (FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()) >= MinCorrectionDistance)
	{
		correction = true;
	}

	if (correction)
	{
		// Construct the corrected move only if we need it.
		FMove CorrectedMove = FMove();
		CorrectedMove.EndVelocity = Velocity;
		CorrectedMove.EndPosition = UpdatedComponent->GetComponentLocation();
		ClientCorrection(CorrectedMove);
	}
	else
	{
		Velocity = Move.EndVelocity;
	}
}

void UPinballMoveComponent::ClientCorrection_Implementation(FMove Move)
{
	//UE_LOG(LogTemp, Warning, TEXT("Client Correction"));
	UpdatedComponent->SetWorldLocation(Move.EndPosition);
	Velocity = Move.EndVelocity;
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
