// Fill out your copyright notice in the Description page of Project Settings.


#include "ProtagonistAnimInstance.h"

#include "ProtagonistCharacter.h"
#include "Component/FootIKComponent.h"
#include "Utils/UtilPlayground.h"

UProtagonistAnimInstance::UProtagonistAnimInstance()
{
}


void UProtagonistAnimInstance::PostInitProperties()
{
	Super::PostInitProperties();
}

void UProtagonistAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	AProtagonistCharacter* Protagonist = Cast<AProtagonistCharacter>(TryGetPawnOwner());
	if (Protagonist)
	{
		FootIKComponent = Cast<UFootIKComponent>(Protagonist->GetComponentByClass(UFootIKComponent::StaticClass()));
		CharacterCurrentInfo = Protagonist->CharacterCurrentInfo;
	}


}

void UProtagonistAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 실행 전 애니메이션 그래프에서 nullptr 참조 방지
	FootIKComponent = NewObject<UFootIKComponent>();
	CharacterCurrentInfo = NewObject<UCharacterCurrentInfo>();

	// UE_LOG(LogTemp, Log, TEXT("UProtagonistAnimInstance::NativeInitializeAnimation"));
}



