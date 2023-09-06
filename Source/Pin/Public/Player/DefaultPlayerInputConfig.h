// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DefaultPlayerInputConfig.generated.h"

/**
 * 
 */
UCLASS()
class PIN_API UDefaultPlayerInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputPush;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* FireGrapple;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* SwivelReticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* FireWeapon;
	
};
