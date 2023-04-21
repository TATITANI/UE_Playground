// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MovementInfo.h"
#include "ProtagonistAnimInstance.generated.h"

/**
 * 
 */

UCLASS()
class PLAYGROUND_API UProtagonistAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// UFUNCTION()
	// void AnimNotify_AttackHit(); // "AnimNotify_노티파이명(AttackHit)" 네이밍 필요

private:
	UPROPERTY( BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UMovementInfo* MovementInfo;
    
	
};
