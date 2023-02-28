// Fill out your copyright notice in the Description page of Project Settings.


#include "PinGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PlayerStartContainer.h"


void APinGameMode::SpawnPlayerPawns()
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStartContainer::StaticClass(), PlayerStarts);
	if (PlayerStarts.Num() > 0)
		PlayerStarts = Cast<APlayerStartContainer>(PlayerStarts[0])->GetPlayerStartArray();
	else
		return;

	for (int i = 0; i < PlayerControllerArray.Num(); i++)
	{
		if (!PlayerStarts.IsValidIndex(i)) continue;
		if (!IsValid(PlayerControllerArray[i]->PlayerPawnClass)) continue;

		PlayerControllerArray[i]->GetPawn()->Destroy();
		APinPlayerPawn* NewPawn = GetWorld()->SpawnActor<APinPlayerPawn>(PlayerControllerArray[i]->PlayerPawnClass, PlayerStarts[i]->GetActorTransform());

		if (!IsValid(NewPawn)) break;

		NewPawn->SetPlayerIndex(i);
		PlayerControllerArray[i]->Possess(NewPawn);
	}
}

void APinGameMode::CreateRemainingControllers()
{
	CleanupPlayerControllers();

	APinPlayerController* NewPlayerController;
	for (int i = PlayerControllerArray.Num(); i <= MaxPlayerIndex; i++)
	{
		NewPlayerController = Cast<APinPlayerController>(UGameplayStatics::CreatePlayer(GetWorld(), -1, true));
		if (IsValid(NewPlayerController))
		{
			NewPlayerController->SetPlayerIndex(i);
			PlayerControllerArray.AddUnique(NewPlayerController);
		}
	}
}

void APinGameMode::CleanupPlayerControllers()
{
	// Get all of the player controllers already in the scene.
	// Set their indecies and add them to the player controller array.
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APinPlayerController::StaticClass(), OutActors);

	int count = 0;
	for(int i = 0; i < OutActors.Num(); i++)
	{
		APinPlayerController* PlayerController = Cast<APinPlayerController>(OutActors[i]);
		if (PlayerController)
		{
			PlayerController->SetPlayerIndex(count);
			PlayerControllerArray.AddUnique(PlayerController);
			count++;
		}
	}

	
	for(int i = 0; i < PlayerControllerArray.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cleaned up player controller %i"), PlayerControllerArray[i]->GetPlayerIndex());
	}
	
}