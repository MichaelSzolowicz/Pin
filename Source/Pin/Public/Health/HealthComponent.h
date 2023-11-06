#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Health/MasterDamageType.h"

#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	void Damage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HurtBoxes)
	TMap<FString, const UPrimitiveComponent*> HurtBoxes;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float CurrentHealth;
		
};
