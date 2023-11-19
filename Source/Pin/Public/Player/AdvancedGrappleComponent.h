#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvancedGrappleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PIN_API UAdvancedGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GrappleCord)
	float Length = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GrappleCord)
	float PullSpeed = 1.0f;

	// Stretching will not be implemented for the first tests
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GrappleCord)
	bool bAllowSlack = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GrappleCord)
	bool bCanStretch = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GrappleCord)
	bool bCanStretchBeyondLength = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Attachment)
	AActor* AttachedTo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PulledBody)
	float PulledMass = 100.0f;

public:	
	UAdvancedGrappleComponent();

	UFUNCTION(BlueprintCallable)
	void AttachTo(AActor* AttachToActor);

	UFUNCTION()
	FVector GrappleForce(float DeltaTime);

	// Getter / Setter
	UFUNCTION(BlueprintCallable)
	void SetPulledMass(float InMass) { PulledMass = InMass; }

	UFUNCTION(BlueprintCallable)
	void SetLenght(float InLength) { Length = InLength; }

	UFUNCTION(BlueprintCallable)
	void SetPullSpeed(float InSpeed) { PullSpeed = InSpeed; }

	UFUNCTION(BlueprintCallable)
	void SetCanStretch(bool InCanStretch) { bCanStretch = InCanStretch; }

	UFUNCTION(BlueprintCallable)
	void SetCanStretchBeyondLength(bool InCanStretchBeyondLength) { bCanStretchBeyondLength = InCanStretchBeyondLength; }

	UFUNCTION(BlueprintCallable)
	float GetPulledMass() { return PulledMass; }

	UFUNCTION(BlueprintCallable)
	float GetLength() { return Length; }

	UFUNCTION(BlueprintCallable)
	float GetPullSpeed() { return PullSpeed; }

	UFUNCTION(BlueprintCallable)
	bool GetCanStretch() { return bCanStretch; }

	UFUNCTION(BlueprintCallable)
	bool GetCanStretchBeyondLength() { return bCanStretchBeyondLength; }

};
