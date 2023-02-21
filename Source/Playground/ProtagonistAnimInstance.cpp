// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtagonistAnimInstance.h"
#include "ProtagonistCharacter.h"


void UProtagonistAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
AProtagonistCharacter *Protagonist = Cast<AProtagonistCharacter>(TryGetPawnOwner());
	MovementInfo = Protagonist->MovementInfo;
}

void UProtagonistAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	
}


void UProtagonistAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
}

