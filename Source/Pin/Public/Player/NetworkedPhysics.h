#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#include "NetworkedPhysics.generated.h"


USTRUCT(BlueprintType)
struct FMove
{
	GENERATED_BODY()

public:
	// Delta time should be changed to a timestamp, server uses diff with previous as delta.
	// Force should, specifically, be the input force. From keys. Grapple input will later 
	// on be calculated server-side if a bool indicating the key is held is sent.
	UPROPERTY()
		float Time = -1.0f;
	UPROPERTY()
		float DeltaTime;
	UPROPERTY()
		FVector Force;
	UPROPERTY()
		FVector EndVelocity;
	UPROPERTY()
		FVector EndPosition;
	UPROPERTY()
		bool bGrapple;
};


/**
 *
 */
UCLASS(Blueprintable)
class PIN_API UNetworkedPhysics : public UMovementComponent
{
	GENERATED_BODY()

public:
	/* Variable related to physics */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector AccumulatedForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector ComponentVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		float Mass = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		float restitution = 1.f;

	/* Variables related to networking */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
		float MinCorrectionDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		TArray<FMove> MovesPendingValidation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		FMove LastValidatedMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		float PrevTimestamp = 0.0f;

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdatePhysics(float DeltaTime);

	void CalcGravity();

	UFUNCTION()
	virtual void PerformMove(FMove Move);

	void ResolveCollision(FHitResult Hit);

	/* Used to check move inputs before executing the move. */
	bool ServerValidateMove(FMove Move);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerPerformMove(FMove Move);
	void ServerPerformMove_Implementation(FMove Move);
	bool ServerPerformMove_Validate(FMove Move);

	UFUNCTION()
	virtual void CheckCompletedMove(FMove Move);

	UFUNCTION(Client, Reliable)
	void ClientCorrection(FMove Move);
	virtual void ClientCorrection_Implementation(FMove Move);

	UFUNCTION(Client, Reliable)
	void ClientApproveMove(float Timestamp);
	void ClientApproveMove_Implementation(float Timestamp);

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	UFUNCTION(BlueprintCallable)
	float InverseMass() { return 1 / Mass; }

	/*Utility functions.*/
		/*
		* Returns the angle between two vectors.
		* @param v1
		* @param v2
		* @return angle
		*/
	float AngleBetweenVectors(FVector v1, FVector v2);

	/*Getter functions.*/
	UFUNCTION(BlueprintCallable)
		float GetSpeed() { return Velocity.Size(); }

};
