#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#include "PinballMoveComponent.generated.h"


USTRUCT(BlueprintType)
struct FMove
{
	GENERATED_BODY()

public:
	// Delta time should be changed to a timestamp, server uses diff with previous as delta.
	// Force should, specifically, be the input force. From keys. Grapple input will later 
	// on be calculated server-side if a bool indicating the key is held is sent.
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
		float MinCorrectionDistance;


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

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerPerformMove(FMove Move);
	void ServerPerformMove_Implementation(FMove Move);
	bool ServerPerformMove_Validate(FMove Move);

	/* Used to check move inputs before executing the move. */
	bool ServerValidateMove(FMove Move);

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
