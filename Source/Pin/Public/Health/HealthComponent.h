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

	UFUNCTION(BlueprintCallable)
	void RegisterHurtBox(UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void RegisterHurtBoxes(TArray<UPrimitiveComponent*> Components);

	UFUNCTION()
	void Damage(TSubclassOf<UMasterDamageType> DamageType, const UPrimitiveComponent* DamagedComponent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HurtBoxes)
	TMap<FString, const UPrimitiveComponent*> HurtBoxes;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = Health)
	float CurrentHealth;
		
};
