#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Bumpers/BumperInterface.h"

#include "PinballPlayer.generated.h"
 

UCLASS()
class PIN_API APinballPlayer : public APawn, public IBumperInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Test)
		float Ping;

	// Input
	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UDefaultPlayerInputConfig* DefaultInputActions;

	// Physics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Physics)
		class UNetworkedPhysics* NetworkPhysics;

	// Scene components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CapsuleComponent)
		class UCapsuleComponent* CapsuleComponent;

	/** Seperates the player's look direction from physics. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RotationRoot)
		class USceneComponent* RotationRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Reticle)
		class UReticle* Reticle;

	// Grapple
	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		float GrappleStrength;

	UPROPERTY(EditDefaultsOnly, Category = Grapple)
		TSubclassOf<AActor> GrappleProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grapple)
		class UStickyProjectile* GrappleProjectileComponent;

	// Weapon
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
		TSubclassOf<AActor> DefaultWeaponProjectile;

public:
	APinballPlayer();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

	virtual void Bump(FVector Impulse) override;

protected:
	void OrientToFloor();

	/**
	* Apply force directly to player via network physics component.
	*/
	UFUNCTION()
	void Push(const FInputActionValue& Value);

	/**
	* Add force directed towards the grapple projectile, if it is attached.
	* If a local player controller, runs every frame until grapple projectile is invalid.
	*/
	UFUNCTION() 
	void AddGrappleForce();

	/**
	* Launch instance of Grapple Projectile Class.
	*/
	void FireGrapple();

	/**
	* Destroy grapple projectile on the client, send rpc to server.
	*/
	void ReleaseGrapple();

	/**
	* Launch Grapple Projectile on server.
	* @param Time - Spawns from the position in network physics' move buffer with nearest timestamp.
	* @param LookAt - Combined forward vector / offset from root.
	*/
	UFUNCTION(Server, Reliable)
	void ServerFireGrapple(float Time, FVector LookAt);
	void ServerFireGrapple_Implementation(float Time, FVector LookAt);

	/**
	* Destroy grapple projectile on server.
	*/
	UFUNCTION(Server, Reliable)
	void ServerReleaseGrapple();
	void ServerReleaseGrapple_Implementation();

	/**
	* Launch instance Default Weapon Projectile.
	*/
	void FireWeapon();

	/**
	* Called when the player releases the fire weapon button.
	*/
	void ReleaseWeapon();

	/**
	* Launch Default Weapon Projectile on server.
	* @param Time - Spawns from the position in network physics' move buffer with nearest timestamp.
	* @param LookAt - Combined forward vector / offset from root.
	*/
	UFUNCTION(Server, Reliable)
	void ServerFireWeapon(float Time, FVector LookAt);
	void ServerFireWeapon_Implementation(float Time, FVector LookAt);

	/**
	* Can be called to perform actions on realse weapon on server.
	*/
	UFUNCTION(Server, Reliable)
	void ServerReleaseWeapon();
	void ServerReleaseWeapon_Implementation();

	/**
	* Update reticle offset using mouse delta.
	*/
	void SwivelReticle(const FInputActionValue& Value);

};
