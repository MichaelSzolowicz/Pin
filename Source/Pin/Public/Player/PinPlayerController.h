// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "Player/PinPlayerPawn.h"

#include "PinPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API APinPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
/*
*	Properties for player tracking.
*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Index")
		int32 PlayerIndex;	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
		TSubclassOf<APinPlayerPawn> PlayerPawnClass;

/*
*	Get / Set.
*/
	UFUNCTION(BlueprintCallable)
		int32 GetPlayerIndex() { return PlayerIndex; }
	UFUNCTION(BlueprintCallable)
		void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }

	UFUNCTION()
		virtual void OnPossess(APawn* InPawn) override;
	
};
