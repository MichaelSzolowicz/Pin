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

	UPROPERTY()
	FTimerHandle Timer;

	/**
	* Start cooldown timer.
	* @param World - use this world's timer manager.
	* @return False if cooldown is already running, true otherwise.
	*/
	bool Start(UWorld* World) {
		if (World->GetTimerManager().IsTimerActive(Timer)) {
			return false;
		}

		World->GetTimerManager().SetTimer(Timer, TimeSeconds, false);
		return true;
	}
	
};
