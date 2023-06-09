#include "OnlineGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PlayerStartContainer.h"
#include "Player/PinPlayerPawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"

void AOnlineGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Rcall GameMode only exists on the server; no need to check if we are on the server here.

	/*TESTONLY*/
	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));

	APawn* PlayerPawn = NewPlayer->GetPawn();
	if(PlayerPawn)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayerPawn->GetName());
	/*ENDTEST*/

	SpawnPlayer(NewPlayer);

}

void AOnlineGameMode::SpawnPlayer_Implementation(APlayerController* NewPlayer)
{
	// Get the payer starts
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStartContainer::StaticClass(), PlayerStarts);
	if (PlayerStarts.Num() > 0)
		PlayerStarts = Cast<APlayerStartContainer>(PlayerStarts[0])->GetPlayerStartArray();
	else
		return;

	// Spawn & posses new pawn
	if (PlayerStarts.Num() >= NumPlayers)
	{
		// Only spawning the default pin player pawn class for now.
		APinPlayerPawn* NewPawn = GetWorld()->SpawnActor<APinPlayerPawn>(DefaultPlayerPawn, PlayerStarts[NumPlayers]->GetActorTransform());
		if (NewPawn)
		{
			/*
			*	As long as the pawn is set to replicate, it will replicate itself with its controller to the client, causing the pawn
			*	to get spawned on the client. Possess() eventually calls PossessedBy() on the pawn, which does set replication to true.
			*	You can override the function inside your custom pawn class to change this behaviour. Setting the class / blueprint to
			*	replicate will cause the pawn to always spawn on the client.
			*/
			NewPlayer->Possess(NewPawn);
			NumPlayers++;
		}
	}
}
