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
};


/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class PIN_API UNetworkedPhysics : public UMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics")
		float MaxAccumulatedForce = 240000.0f;

	// Forces that should be calculated server side. These forces are usually calculated at the start of PerformMove(), after all other relavant forces have been resolved.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
		FVector NaturalForce;

	// Force that gets sent to the server via ServerPerformMove. Should not includ forces like gravity or grapple, which are calculated server side.
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking")
		float MaxDelta = .0166f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		TArray<FMove> MovesPendingValidation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		FMove LastValidatedMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Networking")
		float PrevTimestamp = 0.0f;

public:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
	* @param DeltaTime
	*/
	virtual void UpdatePhysics(float DeltaTime);

	/*
	* Add gravitational force to Accumulated Force.
	*/
	void CalcGravity();

	/**
	* Apply Accumulated Force as movement. Saves the resulting move and send it to the servers.
	* Also applies natural forces. Note this function should later be decomposed into multiple functions.
	* @param Move The move to be performed.
	*/
	UFUNCTION()
	virtual void PerformMove(FMove Move);

	/**
	* Calculates the normal impulse.
	* @param Hit The hit struct we will find the normal impulse for.
	*/
	void ResolveCollision(FHitResult Hit);

	/**
	* RPC to execute and validate a move on the server.
	* @param Move The move to be executed.
	*/
	UFUNCTION(Server, Unreliable)
	void ServerPerformMove(FMove Move);
	void ServerPerformMove_Implementation(FMove Move);

	/**
	* Used to let blueprints know the authorative physics component has received an update.
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnServerReceiveMove();

	/**
	* Used to check move inputs before executing the move.
	* @param Move The move to be checked.
	*/
	bool ServerValidateMove(FMove &Move);

	/**
	* Checks a move executed on the server against the result submitted by the client.
	* Submits a correction if there is a large enough discrepency, approves move otherwise.
	* @param Move The move to check.
	*/
	UFUNCTION()
	virtual void CheckCompletedMove(FMove Move);

	/**
	* Client RPC for receiving correction.
	* @param Move The corrected move.
	*/
	UFUNCTION(Client, Reliable)
	void ClientCorrection(FMove Move);
	virtual void ClientCorrection_Implementation(FMove Move);

	/**
	* Client RPC for receiving move approvals.
	* @param Timestamp The timestamp of the approved move.
	*/
	UFUNCTION(Client, Reliable)
	void ClientApproveMove(float Timestamp);
	void ClientApproveMove_Implementation(float Timestamp);

	/**
	* Add force to Accumulated Force.
	* @param Force The force to add.
	*/
	UFUNCTION(BlueprintCallable)
	void AddForce(FVector Force);

	UFUNCTION(BlueprintCallable)
		void AddInput(FVector2D Input, float Strength);

	UFUNCTION(BlueprintCallable)
	float InverseMass() { return 1 / Mass; }

	/*Utility functions.*/
	/**
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
