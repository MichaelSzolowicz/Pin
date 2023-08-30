#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PinballPlayer.generated.h"
 

UCLASS()
class PIN_API APinballPlayer : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
		class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input)
		class UDefaultPlayerInputConfig* DefaultInputActions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Physics)
		class UNetworkedPhysics* NetworkPhysics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CapsuleComponent)
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RotationRoot)
		class USceneComponent* RotationRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		class USceneComponent* SpawnAt;

public:
	APinballPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
		void Push(const FInputActionValue& Value);

};
