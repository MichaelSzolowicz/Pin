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

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::Damage);
}

void UHealthComponent::Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	const UMasterDamageType* MType = Cast<UMasterDamageType>(DamageType);

	if (IsValid(MType)) {
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	}
	else {

	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("%s took %f damage, %f health remaining"), *GetOwner()->GetName(), Damage, CurrentHealth));
}
