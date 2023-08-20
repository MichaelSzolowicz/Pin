#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AiSteering.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PIN_API UAiSteering : public UActorComponent
{
	GENERATED_BODY()

public:	
	/**
	* Represents the directions our ai can potentially move in.
	* Actual input is a weighted average of each direction, so movement will be smooth even with a low resolution compass.
	*/
	const TArray<FVector> Compass = { {1.f,0.f,0.f}, {.707,-.707f,0.f}, {0.f,-1.f,0.f}, {-.707f,-.707f,0.f}, {-1.f,0.f,0.f}, {-.707f,.707f,0.f}, {0.f,1.f,0.f}, {.707f,.707f,0.f} };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		float PawnDetectionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		float EnvironmentDetectionRadius = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float BaseSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		TEnumAsByte<ECollisionChannel> EnvironmentChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		TEnumAsByte<ECollisionChannel> PawnChannel;

	/** Ai will favor moving away from objects as their distance gets closer or less than this radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float BoundingRadius = 100.0f;

	/** Higher values will cause the ai to more strongly favor moving away from danger close to or less than the bounding radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float BoundsOverlapAvoidanceScale = 1.0f;

	/** Higher values will cause the ai to more strongly favor moving away from non-pawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float EnvironmentAvoidanceScale = 1.0f;

	/** Higher values will cause the ai to more strongly favor moving away from pawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float PawnAvoidanceScale = 1.0f;

	// Sets default values for this component's properties
	UAiSteering();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	* @return Vector representing desired velocity.
	*/
	UFUNCTION(BlueprintCallable)
	FVector GetInput();

protected:
	/**
	* Caculates a vector representing the least desirable direction for the ai to move in.
	* @param OutVector
	*/
	UFUNCTION()
	void CalcDangerVector(FVector& OutVector);


		FVector WeighDanger(FHitResult& Hit, float Radius);


		FVector WeighDanger(TArray<FHitResult> Hits, float Radius);

	UFUNCTION()
		void QuickSphereTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Radius);

	UFUNCTION()
		void CompassTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Distance);

	/**
	* @return Array of directions the ai can potentially move in.
	*/
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetCompass() { return Compass; }
		
};
