// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "TestAiMoveComp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class PIN_API UTestAiMoveComp : public UMovementComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
		void SafeMove(FVector Delta);
	
};
