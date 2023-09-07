#pragma once

#include "CoreMinimal.h"
#include "Projectiles/SimpleProjectile.h"
#include "StickyProjectile.generated.h"


DECLARE_DELEGATE(FOnAttached);


/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class PIN_API UStickyProjectile : public USimpleProjectile
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticky")
		AActor* AttachedTo;
	
public: 
	FOnAttached OnAttached;

	UStickyProjectile();

	const AActor* GetAttachedTo() { return AttachedTo; }

protected:
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult) override;
};
