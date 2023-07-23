#include "Player/PlayerPhysics.h"

#include "Projectiles/StickyProjectile.h"

void UPlayerPhysics::ServerPerformMove_Implementation(FMove Move, bool bGrapple)
{
}

bool UPlayerPhysics::ServerPerformMove_Validate(FMove Move, bool bGrapple)
{
	return true;
}

void UPlayerPhysics::SpawnGrappleProjectile()
{
	AActor* NewProj = GetWorld()->SpawnActor<AActor>(GrappleProjectileClass, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());

	GrappleProjectile = (UStickyProjectile*)(NewProj->GetComponentByClass(UStickyProjectile::StaticClass()));
}
