#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "DamageComponent.generated.h"

class UMasterDamageType;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TSubclassOf<UMasterDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ActorDamageValue = 1.0f;

	UDamageComponent();

	UFUNCTION()
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void RegisterActorAsHitbox();

	UFUNCTION()
	void OnHitBoxOverlap(AActor* OverlappedActor, AActor* OtherActor);
		
	UFUNCTION()
	void OnHitBoxHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	void InflictDamage(AActor* DamagedActor);
};
