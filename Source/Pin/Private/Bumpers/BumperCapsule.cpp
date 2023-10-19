#include "Bumpers/BumperCapsule.h"

#include "Bumpers/BumperInterface.h"

void UBumperCapsule::BeginPlay()
{
	Super::BeginPlay();

	OnComponentHit.AddDynamic(this, &UBumperCapsule::OnHit);
}

void UBumperCapsule::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("%s bumped capsule bumper %s"), *OtherActor->GetName(), *this->GetName());

	IBumperInterface* BumpedObject = Cast<IBumperInterface>(OtherActor);
	if (BumpedObject) {
		BumpedObject->Bump(NormalImpulse * Strength);
	}
}
