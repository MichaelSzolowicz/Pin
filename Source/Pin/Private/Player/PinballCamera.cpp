#include "Player/PinballCamera.h"

#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/CameraRelativePositionControls.h"


APinballCamera::APinballCamera()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmBase = CreateDefaultSubobject<USceneComponent>(TEXT("SpringArmBase"));
	SpringArmBase->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SpringArmBase);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	RelativePositionControls = CreateDefaultSubobject<UCameraRelativePositionControls>(TEXT("RelativePositionControls"));
}

void APinballCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RelativePositionControls->UpdatedComponent = SpringArmBase;
	RelativePositionControls->UpdatePosition();
}

