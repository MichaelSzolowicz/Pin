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
	/** Max speed when unaffected by external forces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float MaxSpeed = 1.0f;

	/** Constant acceleration. Also affects how quickly we can turn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Acceleration = 1.0f;

	/** Braking factor. 1 == instant stopping, 0 == will never stop. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Braking = 1.0f;

	/** Braking factor when external forces cause us to go over max speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BrakingWhileSpeeding = 1.0f;

	/** "Falls" down along Z axis at this constant rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float FallSpeed = 10.0f;

	/** Unit vector representing direction entinty is trying move in. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector InputDirection;

	/** Velocity resulting from input accumulated by Add Input Direction. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector ControlVelocity;

	/** Velocity resulting from forces acting on the Ai, IE AddForce(). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector ExternalVelocity;

	/** Force to be applied next update. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector AccumulatedForce;

	const float TOLERANCE = 0.1f;

public:
	/**
	* Update physics state and location. 
	* @param DeltaTime
	*/
	void Move(float DeltaTime);

	/*
	* Adds param to Input Direction, then normalizes the result.
	* @param Direction - is not normalized before adding, so larger vectors will have more influence.
	*/
	void AddInputDirection(FVector Direction);

	/**
	* Set Input Direction to zero and return a copy of the old Input Direction.
	* @return Input Direction before being reset.
	*/
	FVector ConsumeInputDirection();

	/**
	* @param Force - force to apply
	*/
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	/**
	* @return Sum of External Velocity and Control Velocity.
	*/
	FVector ActualVelocity() { return ExternalVelocity + ControlVelocity; }

	float GetMaxSpeed() { return MaxSpeed; }

protected:
	/**
	* Move down at constant rate. Constrain to plane when hitting a floor.
	* @param DeltaTime
	*/
	void MoveDown(float DeltaTime);

	/**
	* Apply Input as a change in velocity.
	* @param Input - vector to apply, is multiplied by Acceleration.
	* @param DeltaTime
	* @returns Leftover change in velocity.
	*/
	FVector ApplyInput(FVector Input, float DeltaTime);

	/**
	* Applies counter velocity as a fraction of actual velocity.
	*/
	void ApplyBraking(float DeltaTime);

	/**
	* Apply force as a change in velocity.
	* @param DeltaTime
	*/
	UFUNCTION()
	void ApplyAccumulatedForce(float DeltaTime);
	
};
