#include "Player/PinballCamera.h"

#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/CameraPositionVolume.h"


APinballCamera::APinballCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraPositionVolume = CreateDefaultSubobject<UCameraPositionVolume>(TEXT("RelativePositionControls"));
	RootComponent = CameraPositionVolume;

	SpringArmBase = CreateDefaultSubobject<USceneComponent>(TEXT("SpringArmBase"));
	SpringArmBase->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SpringArmBase);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void APinballCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

