#include "Engine/EngineTypes.h"

#include "Health/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
}

void UHealthComponent::RegisterHurtBox(UPrimitiveComponent* Component)
{
	if (IsValid(Component)) {
		HurtBoxes.Add(Component->GetName(), Component);
	}
}

void UHealthComponent::RegisterHurtBoxes(TArray<UPrimitiveComponent*> Components)
{
	for (UPrimitiveComponent* Component : Components) {
		RegisterHurtBox(Component);
	}
}

void UHealthComponent::Damage(TSubclassOf<UMasterDamageType> DamageType, const UPrimitiveComponent* DamagedComponent)
{
	if (!IsValid(DamagedComponent) || !HurtBoxes.Find(DamagedComponent->GetName())) {
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageType.GetDefaultObject()->DefaultDamageValue, 0.0f, MaxHealth);

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("%s took %f damage, %f health remaining"), *DamagedComponent->GetName(), DamageType.GetDefaultObject()->DefaultDamageValue, CurrentHealth));
}
