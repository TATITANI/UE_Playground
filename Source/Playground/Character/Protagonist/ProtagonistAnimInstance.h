// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterCurrentInfo.h"
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

	UProtagonistAnimInstance();
private:
	// class AProtagonistCharacter* Protagonist;

	UPROPERTY(BlueprintReadWrite, Instanced, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UCharacterCurrentInfo* CharacterCurrentInfo;

	UPROPERTY(BlueprintReadWrite, Instanced,Transient,  Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UFootIKComponent* FootIKComponent;


};
