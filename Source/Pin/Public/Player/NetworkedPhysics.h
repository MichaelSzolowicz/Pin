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
		USceneComponent* OrientationRoot;

	// Physics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PhysicsBody")
		class USphereComponent* AngularBody;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
		float Mass = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		float Inertia;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
		float FrictionConstant = .6f;

	UPROPERTY(EditDefaultsOnly, Category = "Physics")
		float restitution = 1.f;

	/**	Total force applied on physics update. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector AccumulatedForce;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector LinearVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector AngularVelocity;

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

	void SetOrientationRoot(USceneComponent* Component);

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
	void AddImpulse(FVector Impulse);

	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	// Getter functions.
	UFUNCTION(BlueprintCallable)
	float GetMass() { return Mass; }

	UFUNCTION(BlueprintCallable)
	float InverseMass() { return 1 / Mass; }

	UFUNCTION(BlueprintCallable)
	float GetInertia() { return Inertia; }

	UFUNCTION(BlueprintCallable)
	float InverseInertia() { 
		if (Inertia <= 0) return 0;
		else return 1 / Inertia;
	}

	UFUNCTION(BlueprintCallable)
	float GetSpeed() { return Velocity.Size(); }

	FMove GetLastValidatedMove() { return LastValidatedMove; }

	FVector GetLookAt() { return PendingLookAt; }

	FMove MoveBufferLast() { return MovesBuffer.Last(); }

	void EstimateMoveFromBuffer(FMove& Move);

protected:
	void CalcIntertia();

	/**
	* Apply Accumulated Force as movement. Save the resulting move and send it to the server.
	* @param DeltaTime
	*/
	virtual void UpdatePhysics(float DeltaTime);	

	/**
	* Add gravitational force to Accumulated Force.
	*/
	void CalcGravity();

	/**
	* Physically moves the updated component. Applies normal impulse.
	* @param Move The move to be performed.
	*/
	UFUNCTION()
	virtual void PerformMove(const FMove& Move);

	/**
	* Calculates the normal impulse.
	* @param Hit The hit struct we will find the normal impulse for.
	*/
	void ResolveCollision(const FHitResult& Hit);

	void ApplyFriction(const FHitResult& Hit);

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
	* Used to check move before execution.
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
