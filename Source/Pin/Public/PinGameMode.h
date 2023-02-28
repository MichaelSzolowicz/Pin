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
	/** I am no longer tracking player pawns since player controllers alrady have a reference to their pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		TArray<APinPlayerPawn*> PlayerPawnArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		TArray<APinPlayerController*> PlayerControllerArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerTracking")
		int32 MaxPlayerIndex;

public:
/** Functions for entering new matches. */
	/** Spawn a player pawn for each player controller for which there exists a player start of the same player index. */
	UFUNCTION(BlueprintCallable)
		void SpawnPlayerPawns();

	/** Create a player controller for each index in MaxPlayerIndex which does not have a controller. */
	UFUNCTION(BlueprintCallable)
		void CreateRemainingControllers();

	UFUNCTION(BlueprintCallable)
		void CleanupPlayerControllers();
	
};
