#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AiSteering.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PIN_API UAiSteering : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Used to trace for level geometry surrounding the ai. */
	const TArray<FVector> Compass = { {1.f,0.f,0.f}, {.707,-.707f,0.f}, {0.f,-1.f,0.f}, {-.707f,-.707f,0.f}, {-1.f,0.f,0.f}, {-.707f,.707f,0.f}, {0.f,1.f,0.f}, {.707f,.707f,0.f} };

	/** Ai can see pawns within this radius. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		float PawnDetectionRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		TEnumAsByte<ECollisionChannel> PawnChannel;

	/** Distance which ai probes against level geometry. Larger values cause the ai to more strongly avoid obstacles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		float EnvironmentDetectionRadius = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
		TEnumAsByte<ECollisionChannel> EnvironmentChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float BaseSpeed = 200.0f;

	/** Higher values will cause the ai to more strongly favor moving away from pawns. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
		float PawnAvoidanceScale = 2.0f;

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
	FVector CalcDangerVector();

	/**
	* Calculate a danger vector for an array of hits.
	* @param Hits
	* @param Radius. Maximum used in normalizing the delta between a hit and us.
	* @return Vector representing least desriable direction and magnitude of the danger.
	*/
	UFUNCTION()
	FVector WeighDanger(TArray<FHitResult> Hits, float Radius);

	UFUNCTION()
	void SphereTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Radius);

	UFUNCTION()
	void CompassTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Distance);

	/**
	* @return Array of directions the ai can potentially move in.
	*/
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetCompass() { return Compass; }
		
};
