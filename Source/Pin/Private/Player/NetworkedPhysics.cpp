#include "Player/NetworkedPhysics.h"

#include "DrawDebugHelpers.h"


void UNetworkedPhysics::BeginPlay()
{
	Super::BeginPlay();

	ComponentVelocity = FVector::Zero();
}


void UNetworkedPhysics::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePhysics(DeltaTime);
}


/**
* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
* Also applies natural forces. Note this function should later be decomposed into multiple functions.
* @param DeltaTime
*/
void UNetworkedPhysics::UpdatePhysics(float DeltaTime)
{
	APawn* Pawn = (APawn*)GetOwner();
	if (!(Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())) {
		return;
	}

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.Time = GetWorld()->TimeSeconds;

	PerformMove(Move);
	Move.EndPosition = GetOwner()->GetActorLocation();
	Move.EndVelocity = ComponentVelocity;
	MovesPendingValidation.Add(Move);

	//Execute move on server
	if (GetNetMode() == NM_Client)
	{
		ServerPerformMove(Move);
	}
}


/*
* Add gravitational force to Natural Force.
*/
void UNetworkedPhysics::CalcGravity()
{
	NaturalForce += FVector(0, 0, GetWorld()->GetGravityZ()) * Mass;
}


/**
* Physically moves the updated component. Applies normal impulse.
* @param Move The move to be performed.
*/
void UNetworkedPhysics::PerformMove(FMove Move)
{
	// Natural forces
	CalcGravity();

	// Delta time
	float dt = Move.Time - PrevTimestamp;
	PrevTimestamp = Move.Time;

	if (dt <= 0) return;

	for (int i = 0; i < 3; i++) {
		FVector Mask = FVector(0,0,0);
		Mask[i]++;
		FVector DeltaPos = ComponentVelocity * dt * Mask;
		// Update position
		FHitResult Hit;
		SafeMoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, Hit);
		// Handle overlaps
		if (Hit.IsValidBlockingHit()) {
			ResolveCollision(Hit);	// Normal impulse.
			SlideAlongSurface(DeltaPos, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}

	ComponentVelocity += ((Move.Force + NaturalForce) / Mass) * dt;
	AccumulatedForce = FVector::Zero();
	NaturalForce = FVector::Zero();
}


/**
* Calculates the normal impulse.
* @param Hit The hit struct we will find the normal impulse for.
*/
void UNetworkedPhysics::ResolveCollision(FHitResult Hit)
{
	// Assuming other actor is static. Later I will need to find a different way to reliably get ComponentVelocity from all types of actor.
	FVector rv = -ComponentVelocity;

	float velAlongNormal = FVector::DotProduct(rv, Hit.Normal);

	if (velAlongNormal < 0) return;

	float e = FMath::Min(restitution, .0f);	/** Need a reliable way to get the restitution for other objects. **/

	// actual impulse.
	float J = -(1 + e) * velAlongNormal;
	J /= InverseMass() + 0;	/* Need a reliable way to get the inverse mass for other objects. */

	// Apply impulse
	FVector Impulse = J * Hit.Normal;
	ComponentVelocity -= InverseMass() * Impulse;
}


/**
* RPC to execute and validate a move on the server.
* @param Move The move to be executed.
*/
void UNetworkedPhysics::ServerPerformMove_Implementation(FMove Move)
{
	OnServerReceiveMove();

	// At this point, we have just received the move should assume Move.Force contains only the force the player is trying to directly add.
	// Natural forces that should be calculated server side, like grapple and gravity, are calculated in PerformMove().
	ServerValidateMove(Move);

	PerformMove(Move);
	CheckCompletedMove(Move);
}


/**
* Used to check move inputs before executing the move.
* @param Move The move to be checked.
*/
bool UNetworkedPhysics::ServerValidateMove(FMove &Move)
{
	if (Move.Force.Size() > MaxAccumulatedForce) {
		Move.Force.Normalize();
		Move.Force *= MaxAccumulatedForce;
		return false;
	}
	return true;
}


/**
* Checks a move executed on the server against the result submitted by the client.
* Submits a correction if there is a large enough discrepency, approves move otherwise.
* @param Move The move to check.
*/
void UNetworkedPhysics::CheckCompletedMove(FMove Move)
{
	// The server's most recent approved position, velocity, etc. is assigned to the components they belong to.
	// The result of the move sent by the client is stored in struct members prefixed by "End."
	bool correction = false;

	if (FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()) > MinCorrectionDistance) {
		correction = true;
	}

	if (correction) {
		Move.EndPosition = UpdatedComponent->GetComponentLocation();
		Move.EndVelocity = ComponentVelocity;
		LastValidatedMove = Move;

		ClientCorrection(Move);
	}
	else {
		LastValidatedMove = Move;
		ClientApproveMove(Move.Time);
	}
}


/**
* Client RPC for receiving correction.
* @param Move The corrected move.
*/
void UNetworkedPhysics::ClientCorrection_Implementation(FMove Move) {
	UpdatedComponent->SetWorldLocation(Move.EndPosition);
	ComponentVelocity = Move.EndVelocity;
	
	int Num = MovesPendingValidation.Num();
	for (int i = 0; i < Num; i++) {
		// Remove all moves prior to the corrected move.
		if (MovesPendingValidation[0].Time <= Move.Time) {
			MovesPendingValidation.RemoveAt(0); 
		}
		else {
			PrevTimestamp = MovesPendingValidation[0].Time;
			break;
		}
	}

	for (int i = 0; i < MovesPendingValidation.Num(); i++) {
		// Execute the remaining moves in moves pending validation.
		PerformMove(MovesPendingValidation[i]);
	}
	// Reset the prev timestamp.
	PrevTimestamp = GetWorld()->TimeSeconds;
}


/**
* Client RPC for receiving move approvals.
* @param Timestamp The timestamp of the approved move.
*/
void UNetworkedPhysics::ClientApproveMove_Implementation(float Timestamp)
{
	// Remove moves that occured before the approved move.
	int Num = MovesPendingValidation.Num();
	for (int i = 0; i < Num; i++) {
		if (MovesPendingValidation[0].Time <= Timestamp) {
			MovesPendingValidation.RemoveAt(0);
		}
		else {
			break;
		}
	}
}


/**
* Add force to Accumulated Force.
* @param Force The force to add.
*/
void UNetworkedPhysics::AddForce(FVector Force)
{
	AccumulatedForce += Force;
}

void UNetworkedPhysics::AddInput(FVector2D Input, float Strength)
{
	FVector AppliedForce = FVector(Input, 0.f);
	AppliedForce.Normalize();
	AccumulatedForce = AppliedForce * Strength;
}


float UNetworkedPhysics::AngleBetweenVectors(FVector v1, FVector v2)
{
	float Dot = FVector::DotProduct(v1, v2);
	float Mag = v1.Size() * v2.Size();
	return PI - (Dot / Mag);
}
