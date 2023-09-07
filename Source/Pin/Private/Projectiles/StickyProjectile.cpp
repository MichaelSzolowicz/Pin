#include "Projectiles/StickyProjectile.h"

UStickyProjectile::UStickyProjectile()
{
	AttachedTo = nullptr;
}

void UStickyProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Sticky Begin Overlap"));

	UpdatedComponent->SetWorldLocation(SweepResult.Location);

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UpdatedComponent->AttachToComponent(OtherActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	AttachedTo = OtherActor;
	Speed = 0.f;

	if (OnAttached.IsBound()) OnAttached.Execute();
}
