#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PinballPlayer.generated.h"
 

UCLASS()
class PIN_API APinballPlayer : public APawn
{
	GENERATED_BODY()

protected:
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
		TSubclassOf<AActor> GrappleProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grapple)
		class UStickyProjectile* GrappleProjectileComponent;

public:
	APinballPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
	void Push(const FInputActionValue& Value);

	void FireGrapple();

};
