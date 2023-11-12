#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Health/MasterDamageType.h"
#include "UObject/CoreNet.h"

#include "HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeDamage, float, DamageValue, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Called any time the component takes damage. */
	UPROPERTY(BlueprintAssignable)
	FOnTakeDamage OnTakeDamage;

	/** Called any time health changes. */
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;

	/** Called when health is reduced to min health. */
	UPROPERTY(BlueprintAssignable)
	FOnHealthDepleted OnHealthDepleted;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Health)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float MinHealth;

	UPROPERTY(EditDefaultsOnly, Category = Health, ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth;

	float PreviousHealth;

public:
	UHealthComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	* Call to damage component.
	* By default binds to Actor on take damage event.
	*/
	UFUNCTION()
	void Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UFUNCTION()
	void OnRep_CurrentHealth();

	/**
	* For internal functionality when health is depleted.
	*/
	void HealthDepleted();

public:
	// Getter - setter functions

	UFUNCTION(BlueprintCallable)
	float GetHealth() { return CurrentHealth; }


};
