#include "Player/PinballMoveComponent.h"

#include "DrawDebugHelpers.h"

void UPinballMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	Velocity = FVector::Zero();
}


void UPinballMoveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePhysics(DeltaTime);
}


void UPinballMoveComponent::UpdatePhysics(float DeltaTime)
{
	CalcGravity();

	// Delta position
	FVector d = Velocity * DeltaTime;
	FVector dx = FVector(d.X, 0, 0);
	FVector dy = FVector(0, d.Y, 0);
	FVector dz = FVector(0, 0, d.Z);

	FVector N = FVector::Zero();

	// x move
	FHitResult OutHit;
	SafeMoveUpdatedComponent(dx, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Red, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{	
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dx, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// y move
	SafeMoveUpdatedComponent(dy, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Green, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dy, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	// z move
	SafeMoveUpdatedComponent(dz, UpdatedComponent->GetComponentRotation(), true, OutHit);

	DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20.f, FColor::Blue, true, .01f);

	if (OutHit.IsValidBlockingHit())
	{
		// Normal force
		N += OutHit.Normal * FVector::DotProduct((-Velocity / DeltaTime), OutHit.Normal);

		SlideAlongSurface(dz, 1.f - OutHit.Time, OutHit.Normal, OutHit);
	}

	N.Normalize();
	N *= FVector::DotProduct((-Velocity / DeltaTime), N);
	AccumulatedForce += N;

	// Acting forces (ignore mass for now, force acts as acceleration)
	Velocity += AccumulatedForce * DeltaTime;

	AccumulatedForce = FVector::Zero();

	//UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Velocity.Size());
}


void UPinballMoveComponent::CalcGravity()
{
	// Ignore mass (for the time being)
	AccumulatedForce += FVector(0, 0, GetWorld()->GetGravityZ());
}

void UPinballMoveComponent::AddForce(FVector Force)
{
	AccumulatedForce += Force;
}

float UPinballMoveComponent::AngleBetweenVectors(FVector v1, FVector v2)
{
	float Dot = FVector::DotProduct(v1, v2);
	float Mag = v1.Size() * v2.Size();
	return PI - (Dot / Mag);
}
