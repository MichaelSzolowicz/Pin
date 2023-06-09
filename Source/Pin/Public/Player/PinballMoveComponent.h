#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#include "PinballMoveComponent.generated.h"


USTRUCT(BlueprintType)
struct FMove
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	FVector Force;
};


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
	float Mass = 100.f;
	float InverseMass;
	float restitution = 1.f;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePhysics(float DeltaTime);

	void UpdatePhysicsWithImpulse(float DeltaTime);

	void ResolveCollision(FHitResult Hit);

	void CalcGravity();

	UFUNCTION()
	void PerformMove(FMove Move);

	UFUNCTION(Server, Reliable)
	void ServerPerformMove(FMove Move);
	void ServerPerformMove_Implementation(FMove Move);

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	float AngleBetweenVectors(FVector v1, FVector v2);
	
};
