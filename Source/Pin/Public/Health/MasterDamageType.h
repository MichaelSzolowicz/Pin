// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "MasterDamageType.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PIN_API UMasterDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Collisions)
	bool bDamageOnHit = true;

	UPROPERTY(EditDefaultsOnly, Category = Collisions)
	bool bDamageOnOverlap = true;

};
