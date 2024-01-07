#include "Player/CameraSpeedRelativeControls.h"

#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PinballPlayer.h"


UCameraSpeedRelativeControls::UCameraSpeedRelativeControls()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCameraSpeedRelativeControls::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCameraSpeedRelativeControls::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

float UCameraSpeedRelativeControls::InterpolateTowards(UCurveFloat* Curve, float Value, float DeltaTime)
{
	float Target = Curve->GetFloatValue(Speed);

	DeltaTime = FMath::Clamp(DeltaTime, 0.0f, 1.0f);

	return (Value * (1.0f - DeltaTime)) + (Target * DeltaTime);
}

