#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BumperInterface.generated.h"

/**
 * 
 */
UINTERFACE()
class PIN_API UBumperInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IBumperInterface
{
	GENERATED_BODY()

public:
	virtual void Bump(FVector Impulse) { return; }
};
