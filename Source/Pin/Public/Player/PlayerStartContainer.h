#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStartContainer.generated.h"

UCLASS()
class PIN_API APlayerStartContainer : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Array")
		TArray<AActor*> Array;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStart")
		TSubclassOf<AActor> PlayerStartClass;

	UFUNCTION(BlueprintCallable, CallInEditor)
		void AddNewStart();
	UFUNCTION(BlueprintCallable, CallInEditor)
		void RemoveStart();

public:
	UFUNCTION()
		TArray<AActor*> GetPlayerStartArray() { return Array; }

};
