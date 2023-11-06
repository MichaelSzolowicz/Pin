#include "Health/HealthComponent.h"

#include "Health/DamageComponent.h"


UDamageComponent::UDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDamageComponent::RegisterHitBox(FHitBox HitBox)
{
	if (IsValid(HitBox.Collider)) {
		HitBoxMap.Add(HitBox.Collider->GetName(), HitBox);
	}

	if (HitBox.DamageType.GetDefaultObject()->bDamageOnOverlap) {
		HitBox.Collider->OnComponentBeginOverlap.AddDynamic(this, &UDamageComponent::OnHitBoxOverlap);
	}
	if (HitBox.DamageType.GetDefaultObject()->bDamageOnHit) {
		HitBox.Collider->OnComponentHit.AddDynamic(this, &UDamageComponent::OnHitBoxHit);
	}
}

void UDamageComponent::RegisterHitBoxes(TArray<FHitBox> HitBoxes)
{
	for (FHitBox HitBox : HitBoxes) {
		RegisterHitBox(HitBox);
	}
}

void UDamageComponent::OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	InflictDamage(OverlappedComponent, OtherComp);
}

void UDamageComponent::OnHitBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	InflictDamage(HitComponent, OtherComp);
}

void UDamageComponent::InflictDamage(UPrimitiveComponent* HitComponent, UPrimitiveComponent* OtherComp)
{
	UHealthComponent* HealthComp = OtherComp->GetOwner()->FindComponentByClass<UHealthComponent>();
	if (!IsValid(HealthComp) || !IsValid(OtherComp)) {
		return;
	}

	FHitBox HitBox = *HitBoxMap.Find(HitComponent->GetName());

	HealthComp->Damage(HitBox.DamageType, OtherComp);
}
