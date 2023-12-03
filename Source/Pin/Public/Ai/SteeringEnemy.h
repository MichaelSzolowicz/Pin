#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SteeringEnemy.generated.h"

UCLASS()
class PIN_API ASteeringEnemy : public APawn
{
	GENERATED_BODY()

protected:
	/** DEPRECATED */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	class UNetworkedPhysics* Physics;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	class UEnemyMovement* Movement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	class UAiSteering* Steering;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BaseSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float BaseAcceleration = 10.0f;

	ASteeringEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	void Move();

};
