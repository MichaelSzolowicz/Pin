#include "Bumpers/BumperCapsule.h"

#include "Bumpers/BumperInterface.h"

void UBumperCapsule::BeginPlay()
{
	Super::BeginPlay();

	OnComponentHit.AddDynamic(this, &UBumperCapsule::OnHit);
}

void UBumperCapsule::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!BumperCooldown.Start(GetWorld())) return;

	UE_LOG(LogTemp, Warning, TEXT("%s bumped capsule bumper %s"), *OtherActor->GetName(), *this->GetName());

	IBumperInterface* BumpedObject = Cast<IBumperInterface>(OtherActor);
	if (BumpedObject) {
		FVector Impulse = FVector::VectorPlaneProject(OtherActor->GetActorLocation() - GetComponentLocation(), GetUpVector());
		Impulse.Normalize();

		UE_LOG(LogTemp, Warning, TEXT("Impulse: %s"), *Impulse.ToString());

		DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + Impulse * 99999, FColor::Yellow, true, 100.0f);

		BumpedObject->Bump(Impulse * Strength);
	}
}
