#include "PawnUtilities.h"


FQuat UPawnUtilities::RotateToFloor(const USceneComponent* RootComponent, float ProbeDistance)
{
	// Probe ground normal
	FHitResult Hit;
	FVector Start = RootComponent->GetComponentLocation();
	FVector End = Start - FVector::UpVector * ProbeDistance;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(RootComponent->GetOwner());

	RootComponent->GetOwner()->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Pawn, CollisionParams);

	FQuat Result = FQuat::Identity;
	if (Hit.bBlockingHit) {
		UE_LOG(LogTemp, Warning, TEXT("Rotation probe hit: %s"), *Hit.GetActor()->GetName());

		Result = FQuat::FindBetweenNormals(RootComponent->GetUpVector(), Hit.Normal);
	}
	
	return Result;
}

bool UPawnUtilities::CollisionFilters(const AActor* ThisActor, const UPrimitiveComponent* OverlappedComponent, const AActor* OtherActor, const UPrimitiveComponent* OtherComponent, bool bIgnoreInstigator)
{
	if (!IsValid(ThisActor)) {
		return false;
	}
	if (bIgnoreInstigator && IsValid(OtherActor) && OtherActor->GetInstigator<APawn>() == ThisActor->GetInstigator<APawn>()) {
		return false;
	}
	return true;
}
