#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#include "PinballMoveComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PIN_API UPinballMoveComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	FVector AccumulatedForce;
	FVector Velocity;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePhysics(float DeltaTime);

	void CalcGravity();

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);
	
};
