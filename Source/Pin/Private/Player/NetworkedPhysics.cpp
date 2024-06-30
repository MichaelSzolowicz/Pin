#include "Player/NetworkedPhysics.h"

#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "DrawDebugHelpers.h"


void UNetworkedPhysics::BeginPlay()
{
	Super::BeginPlay();

	CalcIntertia();

	LinearVelocity = FVector::Zero();
	PendingInput = FVector2D::Zero();
}


void UNetworkedPhysics::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LinearVelocity += AccumulatedImpulse;
	AngularVelocity += AccumulatedAngularImpulse;
	AccumulatedImpulse = FVector::Zero();
	AccumulatedAngularImpulse = FVector::Zero();

	UpdatePhysics(DeltaTime);
}

/**
* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
* @param DeltaTime
*/
void UNetworkedPhysics::UpdatePhysics(float DeltaTime)
{
	APawn* Pawn = (APawn*)GetOwner();
	if (!IsValid(Pawn) || !Pawn->IsLocallyControlled()) {
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
	Move.EndVelocity = LinearVelocity;
	MovesBuffer.Add(Move);

	//Execute move on server
	if (GetNetMode() == NM_Client) {
		if (bShouldUpdateOrientation) {
			ServerPerformMoveWithRotation(InputCopy, Move.Time, Move.EndPosition, PendingLookAt);
		}
		else {
			ServerPerformMove(InputCopy, Move.Time, Move.EndPosition);
		}
	}
}


/**
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

	LinearVelocity += (Move.Force / Mass) * dt;

	FVector DeltaPos = LinearVelocity * dt;

	UWorld* World = GetWorld();
	FVector SweepStart = UpdatedComponent->GetComponentLocation();
	FVector Normal = FVector::Zero();

	for (int i = 0; i < 3; i++) {
		FVector Mask = FVector(0, 0, 0);
		Mask[i]++;
		FVector SweepEnd = SweepStart + DeltaPos * Mask;
		FComponentQueryParams Params = FComponentQueryParams();
		TArray<FHitResult> OutHits;

		const bool bBlockingHit = World->ComponentSweepMulti(OutHits, Cast<UPrimitiveComponent>(UpdatedComponent), SweepStart, SweepEnd, UpdatedComponent->GetComponentRotation(), Params);

		if (bBlockingHit) {
			int32 BlockingHitIndex = -1;
			float BlockingHitNormalDotDelta = UE_BIG_NUMBER;
			for (int HitIndex = 0; HitIndex < OutHits.Num(); HitIndex++) {
				const FHitResult& TestHit = OutHits[HitIndex];

				// UE_LOG(LogTemp, Warning,TEXT("Hit: %s"), *TestHit.GetComponent()->GetFullName());
				// Maybe I should have some kind of aggragate of the normals instead of picking one?

				// Or maybe I should raycast to get if we are grounded and the floor normal, then project these axies into the space of the floor? So this move-by-axis always thinks we are moving on flat ground.
				// But you could collide with a wall in the air, so don't think of this as grounded only functionality.

				if (TestHit.bBlockingHit) {
					if (TestHit.bStartPenetrating) {
						// We may have multiple initial hits, and want to choose the one with the normal most opposed to our movement.
						const float NormalDotDelta = (TestHit.ImpactNormal | (DeltaPos * Mask));
						if (NormalDotDelta < BlockingHitNormalDotDelta)
						{
							BlockingHitNormalDotDelta = NormalDotDelta;
							BlockingHitIndex = HitIndex;
						}
					}
					else if (BlockingHitIndex == -1) {
						BlockingHitIndex = HitIndex;
						break;
					}
				}
			}

			FHitResult BlockingHit = OutHits[BlockingHitIndex];
			ResolveCollisionWithRotation(BlockingHit.ImpactPoint, BlockingHit.ImpactNormal);
		}
	}

	// Update position
	FHitResult Hit;
	MoveUpdatedComponent(DeltaPos, UpdatedComponent->GetComponentRotation(), true, &Hit);
	// Handle overlaps
	if (Hit.IsValidBlockingHit()) {
		// ResolveCollisionWithRotation(Hit.ImpactPoint, Hit.Normal);	// Normal impulse.
		SlideAlongSurface(DeltaPos, 1.f - Hit.Time, Hit.Normal, Hit);	// This really only helps smooth out jitter when goingg uphillat lower fps, but doesn't even entirely solve the issue
	}

	if (bShouldUpdateOrientation) {
		ApplyLookAtOrientation();
	}
}

/**
* Applies normal impulse then calls apply friction.
* Currently assumes the other object is static and has infinite mass.
* @param Hit The hit structure to resolve for.
*/
void UNetworkedPhysics::ResolveCollisionWithRotation(const FVector& ImpactPoint, const FVector& Normal) 
{
	FVector RVector = ImpactPoint - UpdatedComponent->GetComponentLocation();

	FVector Vel = LinearVelocity + AngularVelocity.Cross(RVector);

	float J = Vel.Dot(Normal) * -(1 + FMath::Min(restitution, .5f));

	J /= InverseMass() + 
		FVector::DotProduct(FVector::CrossProduct(InverseInertia() * 
			FVector::CrossProduct(RVector, Normal), RVector),
			Normal);

	FVector Impulse = J * Normal;

	LinearVelocity += InverseMass() * Impulse;
	// AddImpulse(Impulse);

	if (bUseAngularMovement) {
		AngularVelocity += InverseInertia() * RVector.Cross(Impulse);
		//AddAngularImpulse(RVector.Cross(Impulse));
	}

	ApplyFriction(ImpactPoint, Normal, Impulse);
}

/**
* Applies friction impulse.
* Currently assumes the other object is static and has infinite mass.
* @param Hit the hit structre to apply friction for.
*/
void UNetworkedPhysics::ApplyFriction(const FVector& ImpactPoint, const FVector& Normal, const FVector& NormalForce)
{
	FVector RVector = ImpactPoint - UpdatedComponent->GetComponentLocation();
	
	FVector Tangent = LinearVelocity - LinearVelocity.Dot(Normal) * Normal;
	Tangent.Normalize();

	FVector Vel = LinearVelocity + AngularVelocity.Cross(RVector);

	float J = Vel.Dot(Tangent) * -(1);

	J /= InverseMass() +
		FVector::DotProduct(FVector::CrossProduct(InverseInertia() *
			RVector.Cross(Tangent), RVector),
			Tangent);

	if (FMath::Abs(J) > NormalForce.Size() * FrictionConstant) {
		J = NormalForce.Size() * (J / FMath::Abs(J)) * FrictionConstant;
	}
	else {
		J *= FrictionConstant;
	}

	FVector Impulse = J * Tangent;

	LinearVelocity += InverseMass() * Impulse;
	// AddImpulse(Impulse);

	if (bUseAngularMovement) {
		AngularVelocity = InverseInertia() * RVector.Cross(Impulse);

		if (IsValid(AngularBody)) {
			FRotator Rot = UKismetMathLibrary::RotatorFromAxisAndAngle(AngularVelocity.GetSafeNormal(), AngularVelocity.Size());
			AngularBody->AddWorldRotation(Rot);
		}
	}
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


/**
* RPC to execute and validate move with rotation.
* @param InputX
* @param InputY
* @param Time
* @param EndPosition
* @param LookAt Look at rotation.
*/
void UNetworkedPhysics::ServerPerformMoveWithRotation_Implementation(FVector2D Input, float Time, FVector EndPosition, FVector LookAt)
{
	SetLookAtOrientation(LookAt);

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
		if (bUseAngularMovement) {
			ClientCorrectionWithAngularVelocity(UpdatedComponent->GetComponentLocation(), LinearVelocity, AngularVelocity, Move.Time);
		} 
		else {
			ClientCorrection(UpdatedComponent->GetComponentLocation(), LinearVelocity, Move.Time);
		}
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
	LinearVelocity = EndVelocity;
	
	// prolly can change this to a call to clientapprovemove.
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

void UNetworkedPhysics::ClientCorrectionWithAngularVelocity_Implementation(FVector EndPosition, FVector EndLinearVelocity, FVector EndAngularVelocity, float Time)
{
	AngularVelocity = EndAngularVelocity;
	ClientCorrection(EndPosition, EndLinearVelocity, Time);
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


/**
* Find the move with the closet timestamp less than or qual in Moves Buffer.
* @param Move Move to look for. Also holds the reuslting closet move.
*/
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

void UNetworkedPhysics::CalcIntertia()
{
	USphereComponent* Sphere = Cast<USphereComponent>(UpdatedComponent);
	if (!Sphere) {
		bUseAngularMovement = false;
		Inertia = 0;
		return;
	}

	Inertia = (2.f / 5.f) * Mass * Sphere->GetScaledSphereRadius();
}


/**
* Set the component this can update the rotation of.
* @param Component
*/
void UNetworkedPhysics::SetOrientationRoot(USceneComponent* Component)
{
	OrientationRoot = Component;
}


/**
* Set the value of Pending Input.
* @param Input
*/
void UNetworkedPhysics::SetInput(FVector2D Input)
{
	PendingInput = Input;
}


/**
* Sets the value of Pending Look At.
* @param LookAt New Pending Look At.
*/
void UNetworkedPhysics::SetLookAtOrientation(FVector LookAt)
{
	PendingLookAt = LookAt;
}


/*
* Add impulse as an instantenious change in velocity.
*/
void UNetworkedPhysics::AddImpulse(FVector Impulse)
{
	AccumulatedImpulse += InverseMass() * Impulse;
}


void UNetworkedPhysics::AddAngularImpulse(FVector AngularImpulse)
{
	AccumulatedAngularImpulse += InverseInertia() * AngularImpulse;
}


/**
* Add force to accumulated force.
* @param Force
*/
void UNetworkedPhysics::AddForce(FVector Force)
{
	AccumulatedForce += Force;
}


/**
* Adds Pending Input * Input Strength to Accumulated Force, then sets Pending Input to zero.
*/
void UNetworkedPhysics::ApplyInput()
{
	FVector AppliedForce = FVector(PendingInput.X, PendingInput.Y, 0.f);
	if (AppliedForce.Size() > 1.f) AppliedForce.Normalize();
	AddForce(AppliedForce * InputStrength);
	PendingInput = FVector2D::Zero();
}


/**
* Sets Updated Rotation Component world rotation to Pending Look At rotation.
*/
void UNetworkedPhysics::ApplyLookAtOrientation()
{
	UE_LOG(LogTemp, Warning, TEXT("Apply rotation"));
	if (OrientationRoot) {
		FQuat DeltaRotation = OrientationRoot->GetForwardVector().ToOrientationQuat().Inverse() * PendingLookAt.ToOrientationQuat();
		OrientationRoot->AddWorldRotation(DeltaRotation);
	}
}


/**
* Sets AccumulatedForce to zero and returns its previous value.
* @return Accumulated Force
*/
FVector UNetworkedPhysics::ConsumeAccumulatedForce()
{
	FVector Temp = AccumulatedForce;
	AccumulatedForce = FVector::Zero();
	return Temp;
}

/**
* Adds move and removes first move in array if delta is greater than ServerMaxBufferDeltaTime.
* @param Move
*/
void UNetworkedPhysics::AddMoveToServerBuffer(const FMove& Move)
{
	if (GetNetMode() < NM_ListenServer) return;

	MovesBuffer.Add(Move);
	if (MovesBuffer[0].Time - MovesBuffer.Last().Time > ServerBufferMaxDeltaTime) {
		MovesBuffer.RemoveAt(0);
	}
}
