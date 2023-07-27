#include "Player/PlayerPhysics.h"

#include "Projectiles/StickyProjectile.h"


void UPlayerPhysics::SpawnGrappleProjectile()
{
	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());

	GrappleProjectile = (UStickyProjectile*)(NewProj->GetComponentByClass(UStickyProjectile::StaticClass()));
	bIsGrappling = true;
}

void UPlayerPhysics::DespawnGrappleProjectile()
{
	if (GrappleProjectile == nullptr) return;

	GrappleProjectile->GetOwner()->Destroy();
	GrappleProjectile = nullptr;
	bIsGrappling = false;
}

void UPlayerPhysics::UpdatePhysics(float DeltaTime)
{
	//Add natural forces.
	CalcGravity();

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.Time = GetWorld()->TimeSeconds;
	Move.DeltaTime = DeltaTime;
	if (GrappleProjectile && GrappleProjectile->AttachedTo) {
		Move.bGrappleForce = true;
	}
	else {
		Move.bGrappleForce = false;
	}

	//Execute move on client
	APawn* Pawn = (APawn*)GetOwner();
	if (Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())
	{
		PerformMove(Move);
		Move.bGrapple = bIsGrappling;
		Move.EndPosition = GetOwner()->GetActorLocation();
		Move.EndVelocity = ComponentVelocity;

		//Execute move on server
		if (GetNetMode() == NM_Client)
		{
			ServerPerformMoveGrapple(Move);
		}
	}

	if (Move.bGrappleForce) {
		Move.Force += PrevGrappleForce;
	}
	MovesPendingValidation.Add(Move);

}


/**
* Performs default movement, then adds grapple force. Intended to ensure
* the client's final position accounts for the grapple force, but not the force sent to the server.
*/
void UPlayerPhysics::PerformMove(FMove Move)
{
	//UE_LOG(LogTemp, Warning, TEXT("Hello"));
	if (Move.bGrappleForce) {
		FVector direc = (GrappleProjectile->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation());
		direc.Normalize();
		// Move is not passed by ref, so grapple force added on the client will NOT affect the Move sent to the server.
		// Server will calculate its own grapple force, maintaining authority.
		Move.Force += (direc * GrappleStrength);
		PrevGrappleForce = (direc * GrappleStrength);
	}

	Super::PerformMove(Move);
}

void UPlayerPhysics::ServerPerformMoveGrapple_Implementation(FMove Move)
{
	UE_LOG(LogTemp, Warning, TEXT("server move with grapple"));
	if (Move.bGrapple) {
		if (GrappleProjectile == nullptr) {
			SpawnGrappleProjectile();
			GrappleProjectile->SetComponentTickEnabled(false);
			//GrappleProjectile->UpdatePhysics(Move.Time - PrevTimestamp);
		}

		GrappleProjectile->UpdatePhysics(Move.Time - PrevTimestamp);

	}
	else {
		DespawnGrappleProjectile();
	}

	PerformMove(Move);
	CheckCompletedMove(Move);
}
