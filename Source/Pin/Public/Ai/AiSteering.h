#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AiSteering.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PIN_API UAiSteering : public UActorComponent
{
	GENERATED_BODY()

public:	
	const TArray<FVector> Compass = { {1.f,0.f,0.f}, {.707,-.707f,0.f}, {0.f,-1.f,0.f}, {-.707f,-.707f,0.f}, {-1.f,0.f,0.f}, {-.707f,.707f,0.f}, {0.f,1.f,0.f}, {.707f,.707f,0.f} };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ai")
		float BaseSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ai")
		float SightRadius = 500.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
		TArray<FVector> Obstacles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
		TArray<float> DangerWeights;

	FVector DangerVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Navigation")
		float AiBounds = 50.0f;

	// Sets default values for this component's properties
	UAiSteering();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	FVector GetInput();

protected:
	UFUNCTION()
	void GetObstacles(TArray<FVector>& OutObstacles);

	UFUNCTION()
		void CalculateDangerWeights();

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetCompass() { return Compass; }
		
};
