#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UFUNCTION()
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Health)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float CurrentHealth;

protected:
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
		
};
