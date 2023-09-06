#include "Player/NetworkedPhysics.h"

#include "DrawDebugHelpers.h"


void UNetworkedPhysics::BeginPlay()
{
	Super::BeginPlay();

	ComponentVelocity = FVector::Zero();
	PendingInput = FVector2D::Zero();

	//UpdatedRotationComponent = UpdatedComponent;
}


void UNetworkedPhysics::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UE_LOG(LogTemp, Warning, TEXT("Apply rotation"));

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

	// Forces
	CalcGravity();

	// Add input
	FVector2D InputCopy = PendingInput;
	ApplyInput();

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = ConsumeAccumulatedForce();
	Move.Time = GetWorld()->TimeSeconds;

	PerformMove(Move);

	Move.EndPosition = UpdatedComponent->GetComponentLocation();
	Move.EndVelocity = ComponentVelocity;
	MovesBuffer.Add(Move);

	//Execute move on server
	if (GetNetMode() == NM_Client) {
		if (bShouldUpdateRotation) {
			ServerPerformMoveWithRotation(InputCopy, Move.Time, Move.EndPosition, PendingLookAt);
		}
		else {
			ServerPerformMove(InputCopy, Move.Time, Move.EndPosition);
		}
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
void UNetworkedPhysics::PerformMove(const FMove& Move)
{
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

	ComponentVelocity += (Move.Force / Mass) * dt;

	if (bShouldUpdateRotation) {
		ApplyLookAtRotation();
	}
}


/**
* Calculates the normal impulse.
* @param Hit The hit struct we will find the normal impulse for.
*/
void UNetworkedPhysics::ResolveCollision(const FHitResult& Hit)
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
* @param InputX
* @param InputY
* @param Time
* @param EndPosition
*/
void UNetworkedPhysics::ServerPerformMove_Implementation(FVector2D Input, float Time, FVector EndPosition)
{
	CalcGravity();

	SetInput(Input);
	ApplyInput();

	FMove Move = FMove();
	Move.Force = ConsumeAccumulatedForce();
	Move.Time = Time;
	Move.EndPosition = EndPosition;
	Move.LookAt = PendingLookAt;

	if (OnServerReceiveMove.IsBound()) OnServerReceiveMove.Execute();

	ServerValidateMove(Move);
	PerformMove(Move);
	CheckCompletedMove(Move);
}

void UNetworkedPhysics::ServerPerformMoveWithRotation_Implementation(FVector2D Input, float Time, FVector EndPosition, FVector LookAt)
{
	SetLookAtRotation(LookAt);

	ServerPerformMove(Input, Time, EndPosition);
}


/**
* Used to check move inputs before executing the move.
* @param Move The move to be checked.
*/
bool UNetworkedPhysics::ServerValidateMove(const FMove& Move)
{
	return true;
}


/**
* Checks a move executed on the server against the result submitted by the client.
* Submits a correction if there is a large enough discrepency, approves move otherwise.
* @param Move The move to check.
*/
void UNetworkedPhysics::CheckCompletedMove(const FMove& Move)
{
	// The server's most recent approved position, velocity, etc. is assigned to the components they belong to.
	// The result of the move sent by the client is stored in struct members prefixed by "End."
	bool correction = false;

	if (FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()) > MinCorrectionDistance) {
		correction = true;
	}

	if (correction) {
		ClientCorrection(UpdatedComponent->GetComponentLocation(), ComponentVelocity, Move.Time);
	}
	else {
		ClientApproveMove(Move.Time);
	}

	LastValidatedMove = Move;
	AddMoveToServerBuffer(Move);
}


/**
* Client RPC for receiving correction.
* @param EndPosition
* @param EndVelocity
* @param Time
*/
void UNetworkedPhysics::ClientCorrection_Implementation(FVector EndPosition, FVector EndVelocity, float Time) {
	UpdatedComponent->SetWorldLocation(EndPosition);
	ComponentVelocity = EndVelocity;
	
	int Num = MovesBuffer.Num();
	for (int i = 0; i < Num; i++) {
		// Remove all moves prior to the corrected move.
		if (MovesBuffer[0].Time <= Time) {
			MovesBuffer.RemoveAt(0); 
		}
		else {
			PrevTimestamp = MovesBuffer[0].Time;
			break;
		}
	}

	for (int i = 0; i < MovesBuffer.Num(); i++) {
		// Execute the remaining moves in moves pending validation.
		PerformMove(MovesBuffer[i]);
	}
	// Reset the prev timestamp.
	PrevTimestamp = GetWorld()->TimeSeconds;

	FMove Move = FMove();
	Move.EndPosition = EndPosition;
	Move.Time = Time;
	LastValidatedMove = Move;
}


/**
* Client RPC for receiving move approvals.
* @param Timestamp The timestamp of the approved move.
*/
void UNetworkedPhysics::ClientApproveMove_Implementation(float Timestamp)
{
	// Remove moves that occured before the approved move.
	int Num = MovesBuffer.Num();
	for (int i = 0; i < Num; i++) {
		if (MovesBuffer[0].Time <= Timestamp) {
			LastValidatedMove = MovesBuffer[0];
			MovesBuffer.RemoveAt(0);
		}
		else {
			break;
		}
	}
}

void UNetworkedPhysics::EstimateMoveFromBuffer(FMove& Move)
{
	int i = 0;
	float Time = Move.Time;
	for (; i < MovesBuffer.Num(); i++) {
		if (Time < MovesBuffer[i].Time) {
			break;
		}
		Move = MovesBuffer[i];
	}
}

void UNetworkedPhysics::SetUpdatedRotationComponent(USceneComponent* Component)
{
	UpdatedRotationComponent = Component;
	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority) {
		if (Component) {
			UE_LOG(LogTemp, Warning, TEXT("Set rotation root %s"), *Component->GetName());
		}
	}
}


void UNetworkedPhysics::SetInput(FVector2D Input)
{
	PendingInput = Input;
}

void UNetworkedPhysics::SetLookAtRotation(FVector LookAt)
{
	PendingLookAt = LookAt;
}

void UNetworkedPhysics::AddForce(FVector Force)
{
	AccumulatedForce += Force;
}

void UNetworkedPhysics::ApplyInput()
{
	FVector AppliedForce = FVector(PendingInput.X, PendingInput.Y, 0.f);
	if (AppliedForce.Size() > 1.f) AppliedForce.Normalize();
	AddForce(AppliedForce * InputStrength);
	PendingInput = FVector2D::Zero();
}

void UNetworkedPhysics::ApplyLookAtRotation()
{
	UE_LOG(LogTemp, Warning, TEXT("Apply rotation"));
	if (UpdatedRotationComponent) {
		UpdatedRotationComponent->SetWorldRotation(PendingLookAt.Rotation());
	}
}

FVector UNetworkedPhysics::ConsumeAccumulatedForce()
{
	FVector Temp = AccumulatedForce;
	AccumulatedForce = FVector::Zero();
	return Temp;
}

void UNetworkedPhysics::AddMoveToServerBuffer(const FMove& Move)
{
	if (GetNetMode() < NM_ListenServer) return;

	MovesBuffer.Add(Move);
	if (MovesBuffer[0].Time - MovesBuffer.Last().Time > ServerBufferMaxDeltaTime) {
		MovesBuffer.RemoveAt(0);
	}
}
