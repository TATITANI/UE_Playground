// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_BotIsIdle.h"

#include "AIController.h"
#include "Character/Bot/BotCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utils/UtilPlayground.h"

UBTDecorator_BotIsIdle::UBTDecorator_BotIsIdle()
{
	NodeName = TEXT("BotIsIdle");
}

bool UBTDecorator_BotIsIdle::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const ABotCharacter* OwnerBot = Cast<ABotCharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	ensureMsgf(OwnerBot != nullptr, TEXT("UBTDecorator_CanAttack - owner not exsit!"));

	const bool bIdle = OwnerBot->GetCurrentState() == EBotState::Idle &&
		OwnerBot->GetCharacterMovement()->MovementMode == OwnerBot->GetCharacterMovement()->GetGroundMovementMode();

	return bIdle;
}
