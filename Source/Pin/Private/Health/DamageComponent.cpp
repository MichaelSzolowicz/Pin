#include "Health/DamageComponent.h"

#include "Health/HealthComponent.h"


UDamageComponent::UDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageComponent::BeginPlay()
{
	RegisterActorAsHitbox();
}

void UDamageComponent::RegisterActorAsHitbox()
{
	UMasterDamageType* Type = DamageType.GetDefaultObject();

	if (Type->bDamageOnOverlap) {
		GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UDamageComponent::OnHitBoxOverlap);
	}
	if (Type->bDamageOnHit) {
		GetOwner()->OnActorHit.AddDynamic(this, &UDamageComponent::OnHitBoxHit);
	}
}

void UDamageComponent::OnHitBoxOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	InflictDamage(OtherActor);
}

void UDamageComponent::OnHitBoxHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	InflictDamage(OtherActor);
}

void UDamageComponent::InflictDamage(AActor* DamagedActor)
{
	FDamageEvent Event = FDamageEvent();
	Event.DamageTypeClass = DamageType;

	DamagedActor->TakeDamage(ActorDamageValue, Event, GetOwner()->GetInstigatorController<AController>(), GetOwner());
}
