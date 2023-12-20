#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "EnemyMovement.generated.h"

/**
 * Defines simple, velocity based movement for hovering enemies.
 */
UCLASS()
class PIN_API UEnemyMovement : public UMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MaxSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Acceleration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Braking = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Turning = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BrakingWhileSpeeding = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float TurningWhileSpeeding = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector ControlVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector ExcessVelocity;

	const float TOLERANCE = 0.1f;

public:
	void Move(float DeltaTime);

	void AddInputDirection(FVector Direction);

	FVector ConsumeInputDirection();

	/**
	* Force is actually applied next tick, so that it doesn't interfere with ongoing movement calculations.
	* @param Force - force to apply
	*/
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

protected:
	void ApplyInput(FVector Input, float DeltaTime);

	void ApplyBraking(float DeltaTime);

	/**
	* Actual logic for applying a force.
	* Called on a timer by AddForce so force doesn't interfere with ongoing movement calculations.
	*/
	UFUNCTION()
	void AddForceInternal(FVector Force);

public:
	UFUNCTION(BlueprintCallable)
	FVector ActualVelocity() { return ControlVelocity + ExcessVelocity; }
	
};
