#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraSpeedRelativeControls.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIN_API UCameraSpeedRelativeControls : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Speed;

	UCameraSpeedRelativeControls();

protected:

	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	float InterpolateTowards(UCurveFloat* Curve, float Value, float DeltaTime);

		
};
