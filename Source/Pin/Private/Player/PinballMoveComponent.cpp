// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PinballMoveComponent.h"


void UPinballMoveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePhysics(DeltaTime);
}

void UPinballMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	Velocity = FVector::Zero();

	FRotator NewRot = FRotator(-10, 0, 0);
	UpdatedComponent->SetWorldRotation(NewRot, true);
}


void UPinballMoveComponent::UpdatePhysics(float DeltaTime)
{
	static bool bGate = true;
	if (GetOwner()->GetLocalRole() <= ENetRole::ROLE_AutonomousProxy) return;

	float m = 100;
	float g = GetWorld()->GetGravityZ();

	Velocity -= FVector::DownVector * g * DeltaTime;
	FVector dx = Velocity * DeltaTime;

	FHitResult OutHit;
	SafeMoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, OutHit);
	if (OutHit.IsValidBlockingHit())
	{	
		Velocity -= FVector::DownVector * g * DeltaTime * -1;
		FVector N = FMath::Cos(.52f) * g * OutHit.Normal;
		Velocity -= N * DeltaTime;

		SlideAlongSurface(dx, 1.f - OutHit.Time, OutHit.ImpactNormal, OutHit);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Velocity.ToString());
}


void UPinballMoveComponent::CalcGravity()
{
	float g = GetWorld()->GetGravityZ();
	float m = 100;


	AccumulatedForce += FVector(0, 0, m * g);
}

float UPinballMoveComponent::AngleBetweenVectors(FVector v1, FVector v2)
{
	float dot = FVector::DotProduct(v1, v2);
	float mag = v1.Size() * v2.Size();
	return mag;
}
