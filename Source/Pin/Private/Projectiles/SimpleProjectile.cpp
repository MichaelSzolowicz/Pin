#include "Projectiles/SimpleProjectile.h"

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
	GetOwner()->AddActorWorldOffset(GetOwner()->GetActorForwardVector() * Speed * DeltaTime);
}
