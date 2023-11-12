#include "Projectiles/StickyProjectile.h"

#include "PawnUtilities.h"

UStickyProjectile::UStickyProjectile()
{
	AttachedTo = nullptr;
}

void UStickyProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Sticky Begin Overlap"));

	Super::BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!UPawnUtilities::CollisionFilters(GetOwner(), OverlappedComponent, OtherActor, OtherComp)) {
		return;
	}

	if (SweepResult.bBlockingHit) {
		UpdatedComponent->SetWorldLocation(SweepResult.Location);
	}

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules::KeepWorldTransform;

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UpdatedComponent->AttachToComponent(OtherActor->GetRootComponent(), AttachRules);
	AttachedTo = OtherActor;
	Speed = 0.f;

	// Delaying the delegate ensures that the projectile can spawn inside a wall and still execute bindings made during spawning.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UStickyProjectile::PostAttach);
}

void UStickyProjectile::PostAttach()
{
	if (OnAttached.IsBound()) OnAttached.Execute();
}
