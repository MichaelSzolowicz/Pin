// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/TestAiMoveComp.h"

void UTestAiMoveComp::SafeMove(FVector Delta)
{
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
	// Handle overlaps
	if (Hit.IsValidBlockingHit()) {
		//ResolveCollision(Hit);	// Normal impulse.
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

