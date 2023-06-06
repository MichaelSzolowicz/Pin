#include "OnlineGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PlayerStartContainer.h"
#include "Player/PinPlayerPawn.h"

void AOnlineGameMode::PostLogin(APlayerController* NewPlayer)
{
	// Rcall GameMode only exists on the server; no need to check if we are on the server here.

	/*TESTONLY*/
	UE_LOG(LogTemp, Warning, TEXT("PostLogin"));

	APawn* PlayerPawn = NewPlayer->GetPawn();
	if(PlayerPawn)
		UE_LOG(LogTemp, Warning, TEXT("%s"), *PlayerPawn->GetName());
	/*ENDTEST*/

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
			NewPlayer->Possess(NewPawn);
			NumPlayers++;
		}
	}

}