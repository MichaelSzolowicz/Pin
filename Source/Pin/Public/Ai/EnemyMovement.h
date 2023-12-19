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
	float BrakingOverMaxSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float TurningOverMaxSpeed = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector MovementVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector VelocityOverMax;

public:
	void Move(float DeltaTime);

	void AddInputDirection(FVector Direction);

	FVector ConsumeInputDirection();

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);
	
};
