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
	CalculateDangerWeights();

	FVector Input = FVector::Zero();
	for (int i = 0; i < DangerWeights.Num(); i++) {
		Input += Compass[i] * DangerWeights[i] * -1;
	}

	/*TESTONLY*/
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + Input, 5.0f, FLinearColor::Blue);
	UE_LOG(LogTemp, Warning, TEXT("Input Size: %f"), Input.Size());
	/*ENDTEST*/

	Input.Normalize();

	return Input * BaseSpeed;
}

void UAiSteering::GetObstacles(TArray<FVector>& OutObstacles)
{
	FVector Start = GetOwner()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceType;
	TraceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	TraceType.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> OutHits;

	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, Start + FVector(0.0f, 0.f, -.1f), SightRadius, TraceType, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHits, true);

	DangerVector = FVector::Zero();

	for (auto Hit : OutHits) {
		// Normalize the danger vector within our sight radius
		FVector vector = Hit.ImpactPoint - GetOwner()->GetActorLocation();
		float size = vector.Size();
		float scale = (size + SightRadius) / (SightRadius);

		// The closer we are to the danger, the stronger we want to avoid it.
		scale = SightRadius / scale;

		// Multipliers add extra avoidance in special cases.
		if (Cast<APawn>(Hit.GetActor())) scale *= 2;
		if (size <= AiBounds) scale *= 3;

		// Scale and sum the danger vector.
		vector.Normalize();
		vector *= scale;
		DangerVector += vector;
	}

	/*TESTONLY*/
	UE_LOG(LogTemp, Warning, TEXT("Hits: %d"), OutHits.Num());

	for (int i = 0; i < OutHits.Num(); i++) {
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), OutHits[i].ImpactPoint, 20.0f, FLinearColor::Red, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OutHits[i].GetActor()->GetName());
	}
	/*ENDTEST*/

}

void UAiSteering::CalculateDangerWeights()
{
	DangerWeights.Empty();
	for (int i = 0; i < Compass.Num(); i++) {
		DangerWeights.Push(FVector::DotProduct(Compass[i], DangerVector));

		/*TESTONLY*/
		UE_LOG(LogTemp, Warning, TEXT("Weight %d: %f"), i, DangerWeights[i]);
		/*ENDTEST*/
	}
}

