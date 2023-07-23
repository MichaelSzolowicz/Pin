#pragma once

#include "CoreMinimal.h"
#include "Player/NetworkedPhysics.h"
#include "PlayerPhysics.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API UPlayerPhysics : public UNetworkedPhysics
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		TSubclassOf<AActor> GrappleProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		class UStickyProjectile* GrappleProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		float GrappleLiftetimeSeconds;

protected:
	void ServerPerformMove(FMove Move, bool bGrapple);
	void ServerPerformMove_Implementation(FMove Move, bool bGrapple);
	bool ServerPerformMove_Validate(FMove Move, bool bGrapple);

	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();

	
};
