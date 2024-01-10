#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PinballCamera.generated.h"

UCLASS()
class PIN_API APinballCamera : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USceneComponent* SpringArmRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	class UCameraPositionVolume* CameraPositionVolume;
	
public:	
	APinballCamera();

	void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

};
