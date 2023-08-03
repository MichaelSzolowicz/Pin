#pragma once

#include "CoreMinimal.h"
#include "Player/NetworkedPhysics.h"

#include "Projectiles/StickyProjectile.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reticle")
		class UReticle* Reticle;

protected:
	/**
	* Overriden from UNetworkedPhysics. Adds grapple force, then performs move.
	* Intended to make sure client's final position accounts for grapple force while excluding grapple force from the force vector sent to the server.
	* If running on the server, server will calculate its own grapple force.
	* @param Move The move to be executed.
	*/
	void PerformMove(FMove Move) override;

	/**
	* Perform and check a move on the server. Spawns or updates a projectile if bGrapple is true.
	* @param Move The move to be performed and checked.
	* @param bGrapple Pass true if the client is trying to grapple.
	*/
	UFUNCTION(Server, Unreliable)
	void ServerPerformMoveGrapple(FMove Move, bool bGrapple);
	void ServerPerformMoveGrapple_Implementation(FMove Move, bool bGrapple);

	/**
	* Spawn a projectile of GrappleProjectileClass. Always spawns a projectile, always overwrites GrappleProjectile variable.
	* Also sets bIsGrappling to true.
	*/
	UFUNCTION(BlueprintCallable)
	void SpawnGrappleProjectile();

	/*
	* Destroy actor referenced by GrappleProjectile, if it is valid. Also sets bIsGrappling to false.
	*/
	UFUNCTION(BlueprintCallable)
	void DespawnGrappleProjectile();

	/**
	* Overriden from UNetworkedPhysics. Updates physics, but perform specialized operations for grapple force.
	* Calls ServerPerformMOveGrapple() and factors grapple force into saved moves.
	* @param DeltaTime
	*/
	void UpdatePhysics(float DeltaTime) override;
	
};
