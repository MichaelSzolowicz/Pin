#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "CameraPositionVolume.generated.h"

/**
 * 
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class PIN_API UCameraPositionVolume : public UBoxComponent
{
	GENERATED_BODY()

public:
	/**
	* Component to move.
	* Assumed to be an immediate child of Camera Position Volume, reducing need to convert coordinate spaces.
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USceneComponent* UpdatedComponent;

	/** FIXME: Will use game state player array, once that is implemented. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AActor* Player;

protected:
	/** Max distance updated component can move horizontally (along local Y axis). Should be >= 0. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HorizontalLimit;

public: 
	void BeginPlay() override;

	/**
	* Run all logic calculating new local position of Updated Component, then set Updated Component's local position.
	* @return
	*/
	UFUNCTION(BlueprintCallable)
	void UpdatePosition();

	/**
	* Set new horizontal limit for Updated Component.
	* @param NewLimit - should be >= 0.
	*/
	UFUNCTION(BlueprintCallable)
	void SetHorizontalLimit(float NewLimit);

protected:
	/**
	* Calculate new horizontal (local Y) position for Updated Component.
	* @return Y position
	*/
	float GetHorizontalPosition();

	/**
	* Calculate new forward (local X) position for Updated Component.
	* @return X position
	*/
	float GetForwardPosition();

	/**
	* Calculate new vertical (local Z) position for Updated Component.
	* @return Z position
	*/
	float GetVerticalPosition();

};
