// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "CooldownTimer.h"

#include "BumperBox.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class PIN_API UBumperBox : public UBoxComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Bumper)
	float Strength = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = Bumper)
	FCooldownTimer BumperCooldown;

	virtual void BeginPlay() override;

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
};
