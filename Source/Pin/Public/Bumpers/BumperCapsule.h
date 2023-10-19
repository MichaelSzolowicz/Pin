#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "BumperCapsule.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class PIN_API UBumperCapsule : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Bumper)
	float Strength = 1.0f;

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};
