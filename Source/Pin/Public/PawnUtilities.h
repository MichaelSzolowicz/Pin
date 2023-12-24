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
	static bool RotateToFloor(USceneComponent* RootComponent, float ProbeDistance = 250.f);

	UFUNCTION(BlueprintCallable)
	static bool CollisionFilters(const AActor* ThisActor, const UPrimitiveComponent* OverlappedComponent, const AActor* OtherActor, const UPrimitiveComponent* OtherComponent, bool bIgnoreInstigator = true);
	
};
