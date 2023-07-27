#pragma once

#include "CoreMinimal.h"
#include "Player/NetworkedPhysics.h"
#include "PlayerPhysics.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PIN_API UPlayerPhysics : public UNetworkedPhysics
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		TSubclassOf<AActor> GrappleProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		class UStickyProjectile* GrappleProjectile = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		float GrappleLiftetimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		float GrappleStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		bool bIsGrappling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple")
		FVector PrevGrappleForce;

protected:
	void PerformMove(FMove Move) override;

	UFUNCTION(Server, Unreliable)
	void ServerPerformMoveGrapple(FMove Move);
	void ServerPerformMoveGrapple_Implementation(FMove Move);

	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();

	UFUNCTION(BlueprintCallable)
	void DespawnGrappleProjectile();

	void UpdatePhysics(float DeltaTime) override;
	
};
