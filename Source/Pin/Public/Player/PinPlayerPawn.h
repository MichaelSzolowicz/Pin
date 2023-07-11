#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Components/CapsuleComponent.h"

#include "PinPlayerPawn.generated.h"

UCLASS()
class PIN_API APinPlayerPawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PhysicsBody")
		class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
		class UStaticMeshComponent* StaticMesh;

public:	
	APinPlayerPawn(const FObjectInitializer & ObjectInitializer);

};
