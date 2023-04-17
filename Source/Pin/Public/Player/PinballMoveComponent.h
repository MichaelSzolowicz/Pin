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
	float LinearDrag = 1.0f;
	FVector Velocity;
	FVector Acceleration;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePhysics(float DeltaTime);

	void CalcGravity();

	float AngleBetweenVectors(FVector v1, FVector v2);
	
};
