#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"

#include "NetworkedPhysics.generated.h"


USTRUCT(BlueprintType)
struct FMove
{
	GENERATED_BODY()

public:
	UPROPERTY()
		float Time = -1.0f;
	UPROPERTY()
		FVector Force;
	UPROPERTY()
		FVector EndVelocity;
	UPROPERTY()
		FVector EndPosition;
	UPROPERTY()
		FVector LookAt;
};


DECLARE_DELEGATE(FOnServerReceiveMove);


/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class PIN_API UNetworkedPhysics : public UMovementComponent
{
	GENERATED_BODY()

protected:
	// Input
	FVector2D PendingInput;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
		float InputStrength = 60000.f;

	//Rotation
	FVector PendingLookAt;

	/** Send Look At rotation to server if true. */
	UPROPERTY(EditDefaultsOnly, Category = "Rotation")
		bool bShouldUpdateRotation = false;

	/** Update this component's rotation if bShouldUpdateRotation is true. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation")
		USceneComponent* UpdatedRotationComponent;

	// Physics
	UPROPERTY(EditDefaultsOnly, Category = "Physics")
		float Mass = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
		float restitution = 1.f;

	/**	Total force applied on physics update. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector AccumulatedForce;

	/**	DEPRECATED
	*	Forces that should be calculated server side.These forces are usually calculated at the start of PerformMove(), after all other relavant forces have been resolved.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector NaturalForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector ComponentVelocity;

	// Networking
	TArray<FMove> MovesBuffer;

	UPROPERTY(EditDefaultsOnly, Category = "Networking")
		float MinCorrectionDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Networking")
		float ServerBufferMaxDeltaTime = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		FMove LastValidatedMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		float PrevTimestamp = 0.0f;

public:
	FOnServerReceiveMove OnServerReceiveMove;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetUpdatedRotationComponent(USceneComponent* Component);

	/**
	* Submit a 2d input vector to be processed next tick. 
	* @param X
	* @param Y
	*/
	UFUNCTION(BlueprintCallable)
	void SetInput(FVector2D Input);

	UFUNCTION(BlueprintCallable)
	void SetLookAtRotation(FVector LookAt);

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	// Getter functions.

	UFUNCTION(BlueprintCallable)
	float InverseMass() { return 1 / Mass; }

	UFUNCTION(BlueprintCallable)
	float GetSpeed() { return Velocity.Size(); }

	FMove GetLastValidatedMove() { return LastValidatedMove; }

	FMove MoveBufferLast() { return MovesBuffer.Last(); }

	void EstimateMoveFromBuffer(FMove& Move);

protected:
	/**
	* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
	* @param DeltaTime
	*/
	virtual void UpdatePhysics(float DeltaTime);	

	/**
	* Add gravitational force to Accumulated Force.
	*/
	void CalcGravity();

	/**
	* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
	* Also applies natural forces. Note this function should later be decomposed into multiple functions.
	* @param Move The move to be performed.
	*/
	UFUNCTION()
	virtual void PerformMove(const FMove& Move);

	/**
	* Calculates the normal impulse.
	* @param Hit The hit struct we will find the normal impulse for.
	*/
	void ResolveCollision(const FHitResult& Hit);

	/**
	* RPC to execute and validate a move on the server.
	* @param InputX
	* @param InputY
	* @param Time
	* @param EndPosition
	*/
	UFUNCTION(Server, Unreliable)
	void ServerPerformMove(FVector2D Input, float Time, FVector EndPosition);
	void ServerPerformMove_Implementation(FVector2D Input, float Time, FVector EndPosition);

	UFUNCTION(Server, Unreliable)
	void ServerPerformMoveWithRotation(FVector2D Input, float Time, FVector EndPosition, FVector LookAt);
	void ServerPerformMoveWithRotation_Implementation(FVector2D Input, float Time, FVector EndPosition, FVector LookAt);

	/**
	* Used to check move inputs before executing the move.
	* @param Move The move to be checked.
	*/
	bool ServerValidateMove(const FMove& Move);

	/**
	* Checks a move executed on the server against the result submitted by the client.
	* Submits a correction if there is a large enough discrepency, approves move otherwise.
	* @param Move The move to check.
	*/
	UFUNCTION()
	virtual void CheckCompletedMove(const FMove& Move);

	/**
	* Client RPC for receiving correction.
	* @param EndPosition
	* @param EndVelocity
	* @param Time
	*/
	UFUNCTION(Client, Reliable)
	void ClientCorrection(FVector EndPosition, FVector EndVelocity, float Time);
	virtual void ClientCorrection_Implementation(FVector EndPosition, FVector EndVelocity, float Time);

	/**
	* Client RPC for receiving move approvals.
	* @param Timestamp The timestamp of the approved move.
	*/
	UFUNCTION(Client, Reliable)
	void ClientApproveMove(float Timestamp);
	void ClientApproveMove_Implementation(float Timestamp);

	void ApplyInput();

	void ApplyLookAtRotation();

	FVector ConsumeAccumulatedForce();

	void AddMoveToServerBuffer(const FMove& Move);

};
