#include "Player/AdvancedGrappleComponent.h"

#include "DrawDebugHelpers.h"


UAdvancedGrappleComponent::UAdvancedGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAdvancedGrappleComponent::AttachTo(AActor* AttachToActor)
{
	AttachedTo = AttachToActor;
}

/**
* @param Mass - mass of the object being pulled.
* @returns Force required to pull an object of Mass at PullSpeed across a flat surface.
*/
FVector UAdvancedGrappleComponent::GrappleForce(float DeltaTime)
{
	float K = (.5f) * (PulledMass) * FMath::Square(PullSpeed);
	float Force = K / (PullSpeed * DeltaTime);

	FVector OwnerToAttachedTo = AttachedTo->GetActorLocation() - GetOwner()->GetActorLocation();
	FVector Direction = OwnerToAttachedTo.GetSafeNormal();

	if (OwnerToAttachedTo.Size() < Length) {
		//TESTONLY
		DrawDebugLine(GetWorld(), AttachedTo->GetActorLocation(), AttachedTo->GetActorLocation() - Direction * Length, FColor::Blue);

		if (bAllowSlack) {
			return FVector::Zero();
		}
	}
	else {
		GetOwner()->SetActorLocation(AttachedTo->GetActorLocation() - Direction * Length);
	}

	// do not forget about centripetal force if this does not behave as desired.

	//TESTONLY
	DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), AttachedTo->GetActorLocation(), FColor::Red);

	return Force * Direction;
}

