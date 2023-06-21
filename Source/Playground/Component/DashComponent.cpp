// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	ProtagonistCharacter = Cast<AProtagonistCharacter>(GetOwner());
	ensure(ProtagonistCharacter != nullptr);

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(ProtagonistCharacter->GetLocalViewingPlayerController()->InputComponent))
	{
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &UDashComponent::StartDash);
	}
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (RemainDistance > 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::White,
		                                 FString::Printf(TEXT("Dash RemainDistance : %f"), RemainDistance));
		ProgressDash(DeltaTime);
	}
}


void UDashComponent::StartDash(const FInputActionValue& Value)
{
	if (IsCooltime || ProtagonistCharacter->GetCharacterMovement()->MovementMode != MOVE_Walking)
	{
		return;
	}

	RemainDistance = DashDistanceSquared;
	ProtagonistCharacter->GetWorldTimerManager().SetTimer(CooltimeTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		IsCooltime = false;

		ProtagonistCharacter->GetWorldTimerManager().ClearTimer(CooltimeTimerHandle);
	}), CoolTime, false);
}

void UDashComponent::ProgressDash(float& dt)
{
	const float DashCoefficent = DashPowerCurve->GetFloatValue(1 - RemainDistance / DashDistanceSquared);
	const FVector Displacement = ProtagonistCharacter->GetActorForwardVector() * DashPower * DashCoefficent * dt;
	ProtagonistCharacter->AddActorWorldOffset(Displacement);
	RemainDistance -= Displacement.SizeSquared();
}
