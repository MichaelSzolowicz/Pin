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

/*
void UNetworkedPhysics::UpdatePhysics(float DeltaTime)
{
	CalcGravity();

	// Delta position
	FVector d = ComponentVelocity * DeltaTime;
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
		N += OutHit.Normal * FVector::DotProduct((-ComponentVelocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dx, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// y move
	SafeMoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Green, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-ComponentVelocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dy, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// z move
	SafeMoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Blue, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-ComponentVelocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dz, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	N.Normalize();
	N *= FVector::DotProduct((-ComponentVelocity / DeltaTime), N);
	AccumulatedForce += N;

	// Acting forces (ignore mass for now, force acts as acceleration)
	ComponentVelocity += AccumulatedForce * DeltaTime;

	AccumulatedForce = FVector::Zero();
}
*/

void UNetworkedPhysics::UpdatePhysics(float DeltaTime)
{
	//Add natural forces.
	CalcGravity();

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.Time = GetWorld()->TimeSeconds;
	Move.DeltaTime = DeltaTime;

	//Execute move on client
	APawn* Pawn = (APawn*)GetOwner();
	if (Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())
	{
		PerformMove(Move);
		Move.EndPosition = GetOwner()->GetActorLocation();
		Move.EndVelocity = ComponentVelocity;
		MovesPendingValidation.Add(Move);
	}

	//Execute move on server
	if (GetNetMode() == NM_Client)
	{
		ServerPerformMove(Move);
	}
}


void UNetworkedPhysics::CalcGravity()
{
	// Ignore mass (for the time being)
	AccumulatedForce += FVector(0, 0, GetWorld()->GetGravityZ()) * Mass;
}


void UNetworkedPhysics::PerformMove(FMove Move)
{
	// Delta time
	float dt = Move.Time - PrevTimestamp;
	PrevTimestamp = Move.Time;

	ComponentVelocity += (Move.Force / Mass) * dt;
	AccumulatedForce = FVector::Zero();

	FVector d = ComponentVelocity * (dt);
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	// Update position
	// Primitive component uses the normal most opposing the move as the hit norm.
	// Moving each axis indepedently helps ensure every wall applies an opposing force and prevents wall "sticking"
	// I think I could find a better approach, but for now it just needs to get something up and running.
	FHitResult Hit;
	MoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, &Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dx, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Y move
	MoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, &Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dy, 1.f - Hit.Time, Hit.Normal, Hit);
	}

	// Z move
	MoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, &Hit);

	// Handle overlaps
	if (Hit.IsValidBlockingHit())
	{
		ResolveCollision(Hit);
		SlideAlongSurface(dz, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}


void UNetworkedPhysics::ResolveCollision(FHitResult Hit)
{
	// Assuming other actor is static, later I will need to find a different way to reliably get ComponentVelocity from all types of actor.
	FVector rv = -ComponentVelocity;

	double velAlongNormal = FVector::DotProduct(rv, Hit.Normal);

	if (velAlongNormal < 0) return;

	float e = FMath::Min(restitution, .0f);	/** Need a reliable way to get the restitution for other objects. **/

	// actual impulse.
	float J = -(1 + e) * velAlongNormal;
	J /= InverseMass() + 0;	/* Need a reliable way to get the inverse mass for other objects. */

	// Apply impulse
	FVector Impulse = J * Hit.Normal;
	ComponentVelocity -= InverseMass() * Impulse;
}


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


void UNetworkedPhysics::ServerPerformMove_Implementation(FMove Move)
{
	PerformMove(Move);
	CheckCompletedMove(Move);
}


bool UNetworkedPhysics::ServerPerformMove_Validate(FMove Move)
{
	// When declaring a RPC WithValidation, the _Validate function is automatically called and will disconnect the client if it returns false.
	// Only use this feature if you are sure the client is cheating.

	//UE_LOG(LogTemp, Warning, TEXT("Server Validate"));
	return true;
}


void UNetworkedPhysics::CheckCompletedMove(FMove Move)
{
	// The server's most recent approved position, velocity, etc. is assigned to the components they belong to.
	// The result of the move sent by the client is stored in struct members prefixed by "End."
	bool correction = false;

	if (FVector::Distance(Move.EndPosition, UpdatedComponent->GetComponentLocation()) >= MinCorrectionDistance)
	{
		correction = true;
	}

	if (correction)
	{
		Move.EndPosition = UpdatedComponent->GetComponentLocation();
		Move.EndVelocity = ComponentVelocity;
		LastValidatedMove = Move;

		ClientCorrection(Move);
	}
	else
	{
		// Floating point error still has a tendency to lead to desync and corrections, even when the client is sending valid moves.
		// Tried having the server accept the client's velocity if the moves were close enough, but this still breaks server authority.
		//ComponentVelocity = Move.EndVelocity;
		LastValidatedMove = Move;
		ClientApproveMove(Move.Time);
	}
}

void UNetworkedPhysics::ClientCorrection_Implementation(FMove Move)
{
	UE_LOG(LogTemp, Warning, TEXT("Correction %f"), GetWorld()->TimeSeconds);
	UpdatedComponent->SetWorldLocation(Move.EndPosition);
	ComponentVelocity = Move.EndVelocity;
	

	int Num = MovesPendingValidation.Num();
	for (int i = 0; i < Num; i++)
	{
		if (MovesPendingValidation[0].Time <= Move.Time)
		{
			MovesPendingValidation.RemoveAt(0);
		}
		else
		{
			break;
		}
	}
	for (int i = 0; i < MovesPendingValidation.Num(); i++)
	{
		PrevTimestamp = MovesPendingValidation[i].Time - MovesPendingValidation[i].DeltaTime;
		PerformMove(MovesPendingValidation[i]);
	}
	PrevTimestamp = GetWorld()->TimeSeconds;
}

void UNetworkedPhysics::ClientApproveMove_Implementation(float Timestamp)
{
	// Remove moves that occured before the approved move.
	int Num = MovesPendingValidation.Num();
	for (int i = 0; i < Num; i++)
	{
		if (MovesPendingValidation[0].Time <= Timestamp)
		{
			MovesPendingValidation.RemoveAt(0);
		}
		else
		{
			break;
		}
	}
}

void UNetworkedPhysics::AddForce(FVector Force)
{
	//UE_LOG(LogTemp, Warning, TEXT("Velocity"));
	AccumulatedForce += Force;
}

float UNetworkedPhysics::AngleBetweenVectors(FVector v1, FVector v2)
{
	float Dot = FVector::DotProduct(v1, v2);
	float Mag = v1.Size() * v2.Size();
	return PI - (Dot / Mag);
}
