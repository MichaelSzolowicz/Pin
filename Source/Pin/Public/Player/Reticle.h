#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Reticle.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PIN_API UReticle : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Sensitivity = 1.0f;

	// Sets default values for this component's properties
	UReticle();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FVector AddInput(FVector2D Input);

		
};
