#include "Bumpers/BumperBox.h"

#include "Bumpers/BumperInterface.h"
#include "Kismet/KismetMathLibrary.h"

void UBumperBox::BeginPlay()
{
	Super::BeginPlay();

	OnComponentHit.AddDynamic(this, &UBumperBox::OnHit);
}

void UBumperBox::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	IBumperInterface* BumpedObject = Cast<IBumperInterface>(OtherActor);
	if (BumpedObject) {
		FVector Impulse = FVector::Zero();
		FVector LocalActor = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), OtherActor->GetActorLocation());

		UE_LOG(LogTemp, Warning, TEXT("LocalActor: %s"), *LocalActor.ToString());

		if (FMath::Abs(LocalActor.X) - BoxExtent.X > FMath::Abs(LocalActor.Y) - BoxExtent.Y)
		{
			if (LocalActor.X < 0) {
				Impulse = GetForwardVector() * -1;
			}
			else {
				Impulse = GetForwardVector();
			}
			
		}
		if (FMath::Abs(LocalActor.X) - BoxExtent.X < FMath::Abs(LocalActor.Y) - BoxExtent.Y)
		{
			if (LocalActor.Y < 0) {
				Impulse = GetRightVector() * -1;
			}
			else {
				Impulse = GetRightVector();
			}
		}

		DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + Impulse * 99999, FColor::Yellow, false, 1.0f);

		BumpedObject->Bump(Impulse * Strength);
	}
}
