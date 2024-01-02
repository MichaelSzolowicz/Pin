#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraRelativePositionControls.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UCameraRelativePositionControls : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* UpdatedComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AActor* Player;

	UCameraRelativePositionControls();

protected:
	void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void UpdatePosition();
		
};
