// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PinCamera.generated.h"

UCLASS()
class PIN_API APinCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		class UCameraComponent* Camera;

	// Sets default values for this actor's properties
	APinCamera();

};
