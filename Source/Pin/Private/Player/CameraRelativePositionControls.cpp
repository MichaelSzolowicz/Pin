#include "Player/CameraRelativePositionControls.h"

#include "Kismet/GameplayStatics.h"
#include "Player/PinballPlayer.h"


UCameraRelativePositionControls::UCameraRelativePositionControls()
{

}

void UCameraRelativePositionControls::BeginPlay()
{
	Player = UGameplayStatics::GetActorOfClass(GetWorld(), APinballPlayer::StaticClass());
}

void UCameraRelativePositionControls::UpdatePosition()
{
	if (!IsValid(Player)) return;

	FVector NewPos = FVector::Zero();

	NewPos += Player->GetActorLocation() * Player->GetActorForwardVector();
	NewPos += Player->GetActorLocation() * Player->GetActorUpVector();
	
	UpdatedComponent->SetWorldLocation(NewPos);
}
