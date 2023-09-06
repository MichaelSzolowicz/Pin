#include "Projectiles/SimpleProjectile.h"

#include "GameFramework/GameState.h"

// Sets default values for this component's properties
USimpleProjectile::USimpleProjectile()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void USimpleProjectile::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USimpleProjectile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePhysics(DeltaTime);
}

void USimpleProjectile::UpdatePhysics(float DeltaTime)
{
	FHitResult Hit = FHitResult();

	FVector Start = GetOwner()->GetActorLocation();
	FVector DeltaPos = GetOwner()->GetActorForwardVector() * Speed * DeltaTime;
	FCollisionShape CollisionShape = FCollisionShape();
	CollisionShape.MakeSphere(25);
	FCollisionQueryParams CollisionParams = FCollisionQueryParams();
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->SweepSingleByChannel(Hit, Start, Start + DeltaPos, FQuat::Identity, ECollisionChannel::ECC_WorldStatic, CollisionShape, CollisionParams);

	if (Hit.bBlockingHit) {
		GetOwner()->SetActorLocation(Hit.Location);
	}
	else
	{
		GetOwner()->AddActorWorldOffset(DeltaPos, true, &Hit);
	}
}
