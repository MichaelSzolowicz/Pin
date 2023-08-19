#include "Ai/AiSteering.h"

#include "Kismet/KismetSystemLibrary.h"


UAiSteering::UAiSteering()
{

}


// Called when the game starts
void UAiSteering::BeginPlay()
{
	Super::BeginPlay();

}

FVector UAiSteering::GetInput()
{
	GetObstacles(Obstacles);
	return FVector(1.0f, 0.f, 0.f) * BaseSpeed;
}

void UAiSteering::GetObstacles(TArray<FVector>& OutObstacles)
{
	TArray<FVector> ObstaclesBuffer;
	FVector Start = GetOwner()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceType;
	TraceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TraceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> OutHits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, Start + FVector(1.0f, 0.f, 0.f), SightRadius, TraceType, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHits, true);

	UE_LOG(LogTemp, Warning, TEXT("Hits: %d"), OutHits.Num());

	for (int i = 0; i < OutHits.Num(); i++) {
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), OutHits[i].ImpactPoint, 20.0f, FLinearColor::Red, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OutHits[i].GetActor()->GetName());
	}

}

