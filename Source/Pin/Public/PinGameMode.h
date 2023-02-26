// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Player/PinPlayerPawn.h"
#include "Player/PinPlayerController.h"

#include "PinGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API APinGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
/*
*	Properties for player tracking.
*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		TArray<APinPlayerPawn*> PlayerPawnArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		TArray<APinPlayerController*> PlayerControllerArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		int32 MaxPlayerIndex;

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StartMatch();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SpawnRemainingPlayers();
	
};
