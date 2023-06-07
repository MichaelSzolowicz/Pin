#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"

#include "PinPlayerPawn.generated.h"

UCLASS()
class PIN_API APinPlayerPawn : public APawn
{
	GENERATED_BODY()

protected:
/*
*	Properties for player tracking.
*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Index")
		int32 PlayerIndex;

/*
*	Physics related properties.
*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PhysicsBody")
		class UPhysicsBodyComponent* PhysicsBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PhysicsBody")
		class UCapsuleComponent* Capsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PhysicsBody")
		float ScaleInputTorque = 50.0f;

public:	

	APinPlayerPawn(const FObjectInitializer & ObjectInitializer);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;


	UFUNCTION()
		virtual void PossessedBy(AController* NewController) override;

protected:
	UFUNCTION(BlueprintCallable)
		void RollBody();

public:
/*
*	Get / Set.
*/
	UFUNCTION(BlueprintCallable)
		int32 GetPlayerIndex() { return PlayerIndex; }
	UFUNCTION(BlueprintCallable)
		void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }


};
