// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAnimInstance.h"

UBotAnimInstance::UBotAnimInstance()
{
	OnAttackEnded = MakeShared<FOnAttackEnded>();
}

void UBotAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OnMontageEnded.AddDynamic(this, &UBotAnimInstance::MontageEnd);

}

void UBotAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if(auto &&Character = TryGetPawnOwner())
	{
		CurrentVelocity = Character->GetVelocity();
	}
}


void UBotAnimInstance::AnimNotify_AttackHit()
{
	OnAttackHit.Broadcast();
}

void UBotAnimInstance::PlayAttackMontage()
{
	if (Montage_IsPlaying(AttackMontage) == false)
	{
		Montage_Play(AttackMontage);
	}
}

void UBotAnimInstance::MontageEnd(UAnimMontage* Montage, bool bInterrupted) 
{
	if(Montage == AttackMontage)
	{
		OnAttackEnded->Broadcast();		
	}
}

