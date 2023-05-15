// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngameWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UIngameWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true)) // c++ 에서 widget 블루프린트 접근
	class UProgressBar* PB_HpBar; // 에디터 상 컴포넌트 이름과 같아야 함.

	UPROPERTY()
	class AProtagonistCharacter *ProtagonistCharacter;
	
	UFUNCTION()
	void UpdateHp(int32 Hp, int32 MaxHp) const;

protected:
	virtual void NativeOnInitialized() override;
};
