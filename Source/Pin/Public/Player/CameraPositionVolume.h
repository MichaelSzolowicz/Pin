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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USceneComponent* UpdatedComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class AActor* Player;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float HorizontalLimit;

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UpdatePosition();

protected:
	float GetHorizontalPosition();

	float GetForwardPosition();

	float GetVerticalPosition();

};
