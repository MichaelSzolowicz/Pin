#include "Player/PinCamera.h"

#include "Camera/CameraComponent.h"

// Sets default values
APinCamera::APinCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(RootComponent);

}

