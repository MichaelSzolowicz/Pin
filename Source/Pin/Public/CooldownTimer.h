#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CooldownTimer.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PIN_API FCooldownTimer
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeSeconds;

protected:
	UPROPERTY()
	FTimerHandle Timer;

public:
	bool StartCooldown(UWorld* World) {
		if (World->GetTimerManager().IsTimerActive(Timer)) {
			return false;
		}

		World->GetTimerManager().SetTimer(Timer, TimeSeconds, false);
		return true;
	}
	
};
