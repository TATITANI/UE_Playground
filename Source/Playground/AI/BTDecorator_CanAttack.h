// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanAttack.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UBTDecorator_CanAttack : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanAttack();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

private:
	UPROPERTY(EditInstanceOnly, Category=BlackBoard, meta=(AllowPrivateAccess=true))
	FBlackboardKeySelector TargetKeySelector;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess=true))
	float DistanceAttackable = 300.0f;
};
