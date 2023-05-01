// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BotAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UBotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true", BlueprintThreadSafe))
	FVector CurrentVelocity = FVector::ZeroVector;
};
