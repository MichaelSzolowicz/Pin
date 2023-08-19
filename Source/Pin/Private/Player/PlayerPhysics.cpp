#include "Player/PlayerPhysics.h"


/**
* Spawn a projectile of GrappleProjectileClass. Always spawns a projectile.
* Looks for a UStickyProjectile component on the spawned actor. Sets GrappleProjectile equal to this component.
* Also sets bIsGrappling to true.
*/
void UPlayerPhysics::SpawnGrappleProjectile()
{
	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	
	FVector ReticleOffset3D = FVector(ReticleOffset.X, ReticleOffset.Y, 0.0f);
	FRotator Rot = (ReticleOffset3D - FVector::Zero()).Rotation();
	NewProj->SetActorRotation(Rot);

	GrappleProjectile = (UStickyProjectile*)(NewProj->GetComponentByClass(UStickyProjectile::StaticClass()));
	bIsGrappling = true;
}

/*
* Destroy actor which owns GrappleProjectile, if it is valid. Also sets bIsGrappling to false.
*/
void UPlayerPhysics::DespawnGrappleProjectile()
{
	if (GrappleProjectile == nullptr) return;

	GrappleProjectile->GetOwner()->Destroy();
	GrappleProjectile = nullptr;
	bIsGrappling = false;
}

/**
* Overriden from UNetworkedPhysics. Updates physics, but perform specialized operations for grapple force.
* Calls ServerPerformMOveGrapple() and factors grapple force into saved moves.
* @param DeltaTime
*/
void UPlayerPhysics::UpdatePhysics(float DeltaTime)
{
	APawn* Pawn = (APawn*)GetOwner();
	if (!(Pawn && Pawn->Controller && Pawn->Controller->IsLocalPlayerController())) {
		return;
	}

	// Construct the move to be executed.
	FMove Move = FMove();
	Move.Force = AccumulatedForce;
	Move.Time = GetWorld()->TimeSeconds;
	//ReticleOffset = FVector2D(Reticle->GetRelativeLocation().X, Reticle->GetRelativeLocation().Y);

	//Execute move on client
	PerformMove(Move);
	Move.EndPosition = GetOwner()->GetActorLocation();
	Move.EndVelocity = ComponentVelocity;

	//Execute move on server
	if (GetNetMode() == NM_Client) {
		ServerPerformMoveGrapple(Move, bIsGrappling, ReticleOffset);
	}

	// At this point the move has been sent and Move.Force is no longer assumed to be input the player is adding directly.
	// I add the grapple force to the saved move so that it gets recalculated in the event of a correction.
	Move.Force += PrevGrappleForce;
	
	MovesPendingValidation.Add(Move);
}


/**
* Overriden from UNetworkedPhysics. Adds grapple force, then performs move.
* Intended to make sure client's final position accounts for grapple force while excluding grapple force from the force vector sent to the server. 
* If running on the server, server will calculate its own grapple force. 
* @param Move The move to be executed.
*/
void UPlayerPhysics::PerformMove(FMove Move)
{
	if (GetNetMode() == NM_Client) {
		// If we are on the client we only want to apply grapple force if the move IS NOT a correction.
		if (Move.Time >= GetWorld()->TimeSeconds && GrappleProjectile && GrappleProjectile->AttachedTo) {
			FVector direc = (GrappleProjectile->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation());
			direc.Normalize();
			// Grapple force is considered a natural force since it is contextual to the environment, not player input directly.
			NaturalForce += (direc * GrappleStrength);
			PrevGrappleForce = (direc * GrappleStrength);
		}
		else {
			PrevGrappleForce = FVector::Zero();
		}
		Super::PerformMove(Move);
	}
	else {
		// The server will always add grapple force if the conditions are met.
		if (GrappleProjectile && GrappleProjectile->AttachedTo) {
			FVector direc = (GrappleProjectile->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation());
			direc.Normalize();
			// Grapple force is considered a natural force since it is contextual to the environment, not player input directly.
			NaturalForce += (direc * GrappleStrength);
			PrevGrappleForce = (direc * GrappleStrength);
		}
		Super::PerformMove(Move);
	}
}

/**
* Perform and check a move on the server. Spawns or updates a projectile if bGrapple is true.
* @param Move The move to be performed and checked.
* @param bGrapple Pass true if the client is trying to grapple.
*/
void UPlayerPhysics::ServerPerformMoveGrapple_Implementation(FMove Move, bool bGrapple, FVector2D NewReticleOffset)
{
	if (bGrapple) {
		// Spawn grapple projectile if the server does not have one.
		if (GrappleProjectile == nullptr) {
			SpawnGrappleProjectile();
			GrappleProjectile->SetComponentTickEnabled(false);
		}
		// Update the grapple projectile in lockstep with client moves.
		GrappleProjectile->UpdatePhysics(Move.Time - PrevTimestamp);
	}
	else {
		DespawnGrappleProjectile();
	}

	//...

	ReticleOffset = NewReticleOffset;

	ServerPerformMove(Move);
}
