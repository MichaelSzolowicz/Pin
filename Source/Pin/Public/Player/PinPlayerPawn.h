#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PinPlayerPawn.generated.h"

UCLASS()
class PIN_API APinPlayerPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Index")
		int32 PlayerIndex;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		int32 GetPlayerIndex() { return PlayerIndex; }
	UFUNCTION(BlueprintCallable)
		void SetPlayerIndex(int32 NewIndex) { PlayerIndex = NewIndex; }

};
