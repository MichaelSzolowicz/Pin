#include "Player/CameraPositionVolume.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PinballPlayer.h"

void UCameraPositionVolume::BeginPlay()
{
	HorizontalLimit = FMath::Clamp(HorizontalLimit, 0, BoxExtent.Y);

	Player = UGameplayStatics::GetActorOfClass(GetWorld(), APinballPlayer::StaticClass());

	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UCameraPositionVolume::UpdatePosition()
{
	FVector NewPos = FVector(GetForwardPosition(), GetHorizontalPosition(), GetVerticalPosition());
	UpdatedComponent->SetRelativeLocation(NewPos);
}

float UCameraPositionVolume::GetHorizontalPosition()
{
	FVector PlayerRelativePos = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), Player->GetActorLocation());
	float PlayerRelativePosY = FMath::Clamp(PlayerRelativePos.Y, -BoxExtent.Y, BoxExtent.Y);

	return (PlayerRelativePosY / BoxExtent.Y) * HorizontalLimit;
}

float UCameraPositionVolume::GetForwardPosition()
{
	FVector PlayerRelativePos = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), Player->GetActorLocation());
	return PlayerRelativePos.X;
}

float UCameraPositionVolume::GetVerticalPosition()
{
	FVector PlayerRelativePos = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), Player->GetActorLocation());
	return PlayerRelativePos.Z;
}
