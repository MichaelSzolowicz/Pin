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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxRadius = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinRadius = 70.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bConstrainToPlaneNormal = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector PlaneNormal;

	FVector Offset;

public:	
	UFUNCTION(BlueprintCallable)
	FVector AddInput(FVector2D Input);

	void SetPlaneNormal(FVector Normal);

protected:
	void ClampPos();

	void ConstrainToPlaneNormal();
		
};
