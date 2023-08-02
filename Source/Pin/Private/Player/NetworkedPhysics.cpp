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
* @param DeltaTime
*/
void UNetworkedPhysics::UpdatePhysics(float DeltaTime)
{
	APawn* Pawn = (APawn*)GetOwner();
	if (!(Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())) {
		return;
	}

	//Add natural forces.
	CalcGravity();

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.Time = GetWorld()->TimeSeconds;

	//Execute move on client
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
* Add gravitational force to Accumulated Force.
*/
void UNetworkedPhysics::CalcGravity()
{
	AccumulatedForce += FVector(0, 0, GetWorld()->GetGravityZ()) * Mass;
}


/**
* Physically moves the updated component. Applies normal impulse.
* @param Move The move to be performed.
*/
void UNetworkedPhysics::PerformMove(FMove Move)
{
	// Delta time
	float dt = Move.Time - PrevTimestamp;
	PrevTimestamp = Move.Time;

	// Delta position
	FVector d = ComponentVelocity * dt;
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	// Update our position
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


	ComponentVelocity += (Move.Force / Mass) * dt;
	AccumulatedForce = FVector::Zero();
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
	PerformMove(Move);
	CheckCompletedMove(Move);
}


/**
* Used to check move inputs before executing the move.
* @param Move The move to be checked.
*/
bool UNetworkedPhysics::ServerValidateMove(FMove Move)
{
	// Will fill out this function later.
	// I need a better system for determining if a move is valid.
	// I think I should break player input and natural physical forces into seperate inputs.
	// This way the server can validate them seperatly, w/out running into issues like capping the max fall speed when we don't want to.

	// Although in this case we might only need to send over the input. Physical forces should be the same on the server, so we can calculate those
	// using the constants on the server and client provided delta time.
	return false;
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
void UNetworkedPhysics::ClientCorrection_Implementation(FMove Move)
{
	static int numCorrections = 0;
	numCorrections++;

	UE_LOG(LogTemp, Warning, TEXT("Corrections: %d, Time: %f"), numCorrections, GetWorld()->TimeSeconds);

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


float UNetworkedPhysics::AngleBetweenVectors(FVector v1, FVector v2)
{
	float Dot = FVector::DotProduct(v1, v2);
	float Mag = v1.Size() * v2.Size();
	return PI - (Dot / Mag);
}
