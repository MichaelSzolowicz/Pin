#include "Engine/EngineTypes.h"
#include "Net/UnrealNetwork.h"

#include "Health/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	OnRep_CurrentHealth();
	CurrentHealth = MaxHealth;
	PreviousHealth = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::Damage);
}

void UHealthComponent::Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	const UMasterDamageType* MType = Cast<UMasterDamageType>(DamageType);

	if (IsValid(MType)) {
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, MinHealth, MaxHealth);

		OnTakeDamage.Broadcast(Damage, CurrentHealth);
		OnHealthChanged.Broadcast();
	}

	if (CurrentHealth <= MinHealth) {
		HealthDepleted();
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::HealthDepleted()
{
	OnHealthDepleted.Broadcast();
}

void UHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast();
}
