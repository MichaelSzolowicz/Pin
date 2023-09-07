#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PinballPlayer.generated.h"
 

UCLASS()
class PIN_API APinballPlayer : public APawn
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RotationRoot)
		class USceneComponent* RotationRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
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

	bool print = true;

public:
	APinballPlayer();

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void Push(const FInputActionValue& Value);

	UFUNCTION() 
	void AddGrappleForce();

	void FireGrapple();

	void ReleaseGrapple();

	UFUNCTION(Server, Reliable)
	void ServerFireGrapple(float Time, FVector LookAt);
	void ServerFireGrapple_Implementation(float Time, FVector LookAt);

	UFUNCTION(Server, Reliable)
	void ServerReleaseGrapple();
	void ServerReleaseGrapple_Implementation();

	void FireWeapon();

	void ReleaseWeapon();

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon(float Time, FVector LookAt);
	void ServerFireWeapon_Implementation(float Time, FVector LookAt);

	UFUNCTION(Server, Reliable)
	void ServerReleaseWeapon();
	void ServerReleaseWeapon_Implementation();

	void SwivelReticle(const FInputActionValue& Value);

};
