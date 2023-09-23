#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PawnUtilities.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API UPawnUtilities : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FQuat RotateToFloor(const USceneComponent* RootComponent, float ProbeDistance = 250.f);
	
};
