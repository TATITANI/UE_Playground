// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProtagonistAnimInstance.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, BlueprintType)
class PLAYGROUND_API UProtagonistAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;

	
};
