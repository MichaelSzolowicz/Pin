#include "Player/AdvancedGrappleComponent.h"


UAdvancedGrappleComponent::UAdvancedGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAdvancedGrappleComponent::AttachTo(AActor* AttachToActor)
{
	AttachedTo = AttachToActor;
}

FVector UAdvancedGrappleComponent::GrappleForc()
{
	return FVector();
}

