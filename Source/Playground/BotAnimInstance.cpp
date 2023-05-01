// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBotAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UBotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(auto Character = TryGetPawnOwner())
	{
		CurrentVelocity = Character->GetVelocity();
	}
}

