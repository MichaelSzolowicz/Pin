#include "Player/CameraPositionVolume.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PinballPlayer.h"

void UCameraPositionVolume::BeginPlay()
{
	HorizontalLimit = FMath::Clamp(HorizontalLimit, 0, BoxExtent.Y);

	/* FIXME: Will use game state player array, once that is implemented. */
	Player = UGameplayStatics::GetActorOfClass(GetWorld(), APinballPlayer::StaticClass());

	// Volume is used for measurement only; it should not have collision.
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UCameraPositionVolume::UpdatePosition()
{
	FVector NewPos = FVector(GetForwardPosition(), GetHorizontalPosition(), GetVerticalPosition());
	UpdatedComponent->SetRelativeLocation(NewPos);
}

float UCameraPositionVolume::GetHorizontalPosition()
{
	// Calculate the players realtive pos, bounded to the volume.
	// Camera will move left or right some % of its Horizontal Limit, where some % is the % the player is from the center towards the edge.
	FVector PlayerRelativePos = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), Player->GetActorLocation());
	float PlayerRelativePosY = FMath::Clamp(PlayerRelativePos.Y, -BoxExtent.Y, BoxExtent.Y);

	return (PlayerRelativePosY / BoxExtent.Y) * HorizontalLimit;
}

float UCameraPositionVolume::GetForwardPosition()
{
	// Camera tracks player X position 1 : 1.
	FVector PlayerRelativePos = UKismetMathLibrary::InverseTransformLocation(GetComponentTransform(), Player->GetActorLocation());
	return PlayerRelativePos.X;
}

float UCameraPositionVolume::GetVerticalPosition()
{
	// For now the local z position will be fixed so the camera moves along a flat plane.
	return UpdatedComponent->GetRelativeLocation().Z;
}

void UCameraPositionVolume::SetHorizontalLimit(float NewLimit)
{
	HorizontalLimit = FMath::Clamp(NewLimit, 0, BoxExtent.Y);
}
