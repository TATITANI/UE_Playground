// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtagonistAnimInstance.h"
#include "ProtagonistCharacter.h"

void UProtagonistAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
}

void UProtagonistAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UProtagonistAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	AProtagonistCharacter* Protagonist = Cast<AProtagonistCharacter>(TryGetPawnOwner());
	if (Protagonist)
	{
		MovementInfo = Protagonist->MovementInfo;
		UE_LOG(LogTemp, Log, TEXT("NativeInitializeAnimation"));
		
	}
}


void UProtagonistAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}
