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
	UPROPERTY()
	FVector EndVelocity;
	UPROPERTY()
	FVector EndPosition;
};


/**
 * 
 */
UCLASS(Blueprintable)
class PIN_API UPinballMoveComponent : public UMovementComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Params")
	float MaxAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Params")
	float MaxSpeed;


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

	UFUNCTION(Server, Unreliable)
	void ServerPerformMove(FMove Move);
	void ServerPerformMove_Implementation(FMove Move);

	UFUNCTION()
		void CheckCompletedMove(FMove Move);

	UFUNCTION(Client, Reliable)
		void ClientCorrection(FMove Move);
	void ClientCorrection_Implementation(FMove Move);

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	float AngleBetweenVectors(FVector v1, FVector v2);

	UFUNCTION(BlueprintCallable)
		float GetSpeed() { return Velocity.Size(); }
	
};
