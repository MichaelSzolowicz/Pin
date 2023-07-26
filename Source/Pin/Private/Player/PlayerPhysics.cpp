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
	Move.bGrapple = bIsGrappling;

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
		ServerPerformMoveGrapple(Move);
	}
}

/**
* Performs default movement, then adds grapple force. Intended to ensure
* the client's final position accounts for the grapple force, but not the force sent to the server.
*/
void UPlayerPhysics::PerformMove(FMove Move)
{
	if (GrappleProjectile && GrappleProjectile->AttachedTo) {
		FVector direc = (GrappleProjectile->GetOwner()->GetActorLocation() - GetOwner()->GetActorLocation());
		direc.Normalize();
		ComponentVelocity += (direc * GrappleStrength / Mass) * (Move.Time - PrevTimestamp);
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
		}
		else {
			GrappleProjectile->UpdatePhysics(Move.Time - PrevTimestamp);
		}
	}
	else {
		DespawnGrappleProjectile();
	}

	ServerPerformMove(Move);
}
