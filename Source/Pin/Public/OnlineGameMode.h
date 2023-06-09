// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OnlineGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API AOnlineGameMode : public AGameModeBase
{
	GENERATED_BODY()


public:
	/*FIXME: Test Only*/
	int32 NumPlayers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerPawn")
		TSubclassOf<APawn> DefaultPlayerPawn;

	UFUNCTION()
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(NetMulticast, Reliable)
		void SpawnPlayer(APlayerController* NewPlayer);
	void SpawnPlayer_Implementation(APlayerController* NewPlayer);
	
};
