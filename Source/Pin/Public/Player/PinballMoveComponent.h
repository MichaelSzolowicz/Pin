#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

// Moving functionality to NetworkedPhysics. For now include the class so we can still access FMove struct.
#include "Player/NetworkedPhysics.h"

#include "PinballMoveComponent.generated.h"


/**	
 * DEPRECATED
 * Moving functionality to NetworkedPhysics.
 */
UCLASS(Blueprintable)
class PIN_API UPinballMoveComponent : public UMovementComponent
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

	void UpdatePhysics(float DeltaTime);

	void CalcGravity();

	UFUNCTION()
	void PerformMove(FMove Move);

	void ResolveCollision(FHitResult Hit);

	/* Used to check move inputs before executing the move. */
	bool ServerValidateMove(FMove Move);

	UFUNCTION(Server, Unreliable, WithValidation)
	virtual void ServerPerformMove(FMove Move);
	virtual void ServerPerformMove_Implementation(FMove Move);
	virtual bool ServerPerformMove_Validate(FMove Move);

	UFUNCTION()
	void CheckCompletedMove(FMove Move);

	UFUNCTION(Client, Reliable)
	void ClientCorrection(FMove Move);
	void ClientCorrection_Implementation(FMove Move);

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
