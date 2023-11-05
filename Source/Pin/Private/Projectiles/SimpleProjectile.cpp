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

	UpdatedComponent = GetOwner()->GetRootComponent();

	//OnComponentBeginOverlap.AddDynamic(this, &USimpleProjectile::BeginOverlap);
}

void USimpleProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GetOwner()->SetActorLocation(GetComponentLocation());
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
	FCollisionShape CollisionShape = GetCollisionShape();
	FCollisionQueryParams CollisionParams = FCollisionQueryParams();
	CollisionParams.AddIgnoredActor(GetOwner());

	UpdatedComponent->AddWorldOffset(DeltaPos, true, &Hit);

	if (Hit.bBlockingHit) {
		UpdatedComponent->SetWorldLocation(Hit.Location);
	}
}
