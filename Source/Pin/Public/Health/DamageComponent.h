#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Containers/Map.h"

#include "DamageComponent.generated.h"

class UMasterDamageType;

USTRUCT(Blueprintable)
struct PIN_API FHitBox
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPrimitiveComponent* Collider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMasterDamageType> DamageType;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageComponent();

protected:
	UPROPERTY(BlueprintReadWrite, Category = HitBoxes)
	TMap<FString, FHitBox> HitBoxMap;

	UFUNCTION(BlueprintCallable)
	void RegisterHitBox(FHitBox HitBox);

	UFUNCTION(BlueprintCallable)
	void RegisterHitBoxes(TArray<FHitBox> HitBoxes);

	UFUNCTION()
	void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
	void OnHitBoxHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InflictDamage(UPrimitiveComponent* HitComponent, UPrimitiveComponent* OtherComp);
};
