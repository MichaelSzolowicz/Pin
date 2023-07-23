#pragma once

#include "CoreMinimal.h"
#include "Projectiles/SimpleProjectile.h"
#include "StickyProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API UStickyProjectile : public USimpleProjectile
{
	GENERATED_BODY()
	
public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticky")
		AActor* AttachedTo;

	UStickyProjectile();
};
