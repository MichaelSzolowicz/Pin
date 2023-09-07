#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "SimpleProjectile.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PIN_API USimpleProjectile : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics");
	float Speed;

	// Sets default values for this component's properties
	USimpleProjectile();

protected:
	USceneComponent* UpdatedComponent;

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdatePhysics(float DeltaTime);


};