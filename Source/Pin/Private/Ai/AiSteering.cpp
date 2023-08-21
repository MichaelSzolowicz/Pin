#include "Ai/AiSteering.h"

#include "Kismet/KismetSystemLibrary.h"


UAiSteering::UAiSteering()
{
	EnvironmentChannel = ECollisionChannel::ECC_WorldStatic;
	PawnChannel = ECollisionChannel::ECC_Pawn;
}


// Called when the game starts
void UAiSteering::BeginPlay()
{
	Super::BeginPlay();

}


/**
* @return Vector representing desired velocity.
*/
FVector UAiSteering::GetInput()
{
	FVector DangerVector = FVector::Zero();

	CalcDangerVector(DangerVector);

	FVector Input = FVector::Zero();
	for (int i = 0; i < Compass.Num(); i++) {
		Input += Compass[i] * FVector::DotProduct(Compass[i], DangerVector) * -1;
	}

	Input.Z = 0.f;

	/*TESTONLY*/
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + Input, 5.0f, FLinearColor::Blue);
	UE_LOG(LogTemp, Warning, TEXT("Input Size: %f"), Input.Size());
	/*ENDTEST*/

	Input.Normalize();

	return Input * BaseSpeed;
}


/**
* Caculates a vector representing the least desirable direction for the ai to move in.
* @param OutVector
*/
void UAiSteering::CalcDangerVector(FVector& OutVector)
{
	TArray<FHitResult> OutHits;
	FVector Geometry = FVector::Zero();
	FVector Pawns = FVector::Zero();

	CompassTrace(OutHits, EnvironmentChannel, EnvironmentDetectionRadius);
	OutVector += WeighDanger(OutHits, EnvironmentDetectionRadius);

	QuickSphereTrace(OutHits, PawnChannel, PawnDetectionRadius);
	OutVector += WeighDanger(OutHits, PawnDetectionRadius);

	/*TESTONLY*/
	UE_LOG(LogTemp, Warning, TEXT("Hits: %d"), OutHits.Num());

	for (int i = 0; i < OutHits.Num(); i++) {
		//UKismetSystemLibrary::DrawDebugPoint(GetWorld(), OutHits[i].ImpactPoint, 20.0f, FLinearColor::Red, 1.0f);
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *OutHits[i].GetActor()->GetName());
	}
	/*ENDTEST*/

}

FVector UAiSteering::WeighDanger(FHitResult& Hit, float Radius)
{
	// Normalize the delta vector using sight radius as the upper bound.
	FVector Danger = Hit.ImpactPoint - GetOwner()->GetActorLocation();
	float size = Danger.Size();
	float Weight = size / Radius;

	// The closer we are to the danger, the stronger we want to avoid it.
	Weight = 1.f - Weight;

	// Multipliers add extra avoidance in special cases.
	if (Cast<APawn>(Hit.GetActor())) Weight *= PawnAvoidanceScale;

	// Scale and sum the danger vector.
	Danger.Normalize();
	return Danger * Weight;
}

FVector UAiSteering::WeighDanger(TArray<FHitResult> Hits, float Radius)
{
	FVector Danger = FVector::Zero();
	for (auto Hit : Hits) {
		Danger += WeighDanger(Hit, Radius);
	}
	return Danger;
}

void UAiSteering::QuickSphereTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Radius)
{
	TArray<FHitResult> HitBuffer;
	FVector Start = GetOwner()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> ActorsToIgnore;

	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ObjectType));
	ActorsToIgnore.Add(GetOwner());

	UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, Start + FVector(0.0f, 0.f, -.1f), Radius, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, OutHits, true);
}

void UAiSteering::CompassTrace(TArray<FHitResult>& OutHits, ECollisionChannel ObjectType, float Distance)
{
	TArray<FHitResult> HitBuffer;
	FVector Start = GetOwner()->GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> ActorsToIgnore;

	OutHits.Empty();
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ObjectType));
	ActorsToIgnore.Add(GetOwner());

	for (int i = 0; i < Compass.Num(); i++) {
		UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), Start, Start + Compass[i] * Distance, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitBuffer, true);
		OutHits.Append(HitBuffer);
	}
}

