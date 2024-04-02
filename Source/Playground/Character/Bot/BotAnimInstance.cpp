// Fill out your copyright notice in the Description page of Project Settings.


#include "BotAnimInstance.h"

#include "BotCharacter.h"
#include "Utils/UtilPlayground.h"

UBotAnimInstance::UBotAnimInstance()
{
}

void UBotAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OnMontageEnded.AddUniqueDynamic(this, &UBotAnimInstance::MontageEnd);
	OwnerBot = Cast<ABotCharacter>(TryGetPawnOwner());
	ensure(OwnerBot!= nullptr);
	
}

void UBotAnimInstance::AnimNotify_AttackHit()
{
	OnAttackHit.Broadcast();
}

void UBotAnimInstance::AnimNotify_StandUp()
{
	OwnerBot->SetState(EBotState::Idle);
}

void UBotAnimInstance::PlayBotMontage(UAnimMontage* Montage, EBotState::Type ResetState)
{
	Montage_Play(Montage);
	BotStateByMontage.FindOrAdd(Montage, ResetState);
}

void UBotAnimInstance::PlayAttackMontage(EBotState::Type ResetState)
{
	PlayBotMontage(AttackMontage, ResetState);
}

void UBotAnimInstance::PlayAttackedMontage(EBotState::Type ResetState)
{
	PlayBotMontage(AttackedMontage, ResetState);
}

void UBotAnimInstance::MontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (BotStateByMontage.Contains(Montage))
	{
		OwnerBot->ResetCurrentState(BotStateByMontage[Montage]);
	}

	if (Montage == AttackMontage)
	{
		OwnerBot->OnAttackEnd.Broadcast();
	}
}
