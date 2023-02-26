// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PinPlayerStart.generated.h"

UCLASS()
class PIN_API APinPlayerStart : public AActor
{
	GENERATED_BODY()
	
public:	
/*
*	Properties for player tracking.
*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Index")
		int32 PlayerIndex;

};
