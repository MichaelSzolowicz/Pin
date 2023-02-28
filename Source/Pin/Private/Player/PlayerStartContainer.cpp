#include "Player/PlayerStartContainer.h"

void APlayerStartContainer::AddNewStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Player start container adds new start."));

	AActor* NewPlayerStart = GetWorld()->SpawnActor<AActor>(PlayerStartClass, GetActorTransform());

	if (IsValid(NewPlayerStart)) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *NewPlayerStart->GetName());

		Array.Add(NewPlayerStart);
	}
}

void APlayerStartContainer::RemoveStart()
{

}
