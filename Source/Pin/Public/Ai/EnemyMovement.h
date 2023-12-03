// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "EnemyMovement.generated.h"

/**
 * 
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector InputDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	FVector MovementVelocity;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Move();

	void AddInputDirection(FVector Direction);

	FVector ConsumeInputDirection();
	
};
