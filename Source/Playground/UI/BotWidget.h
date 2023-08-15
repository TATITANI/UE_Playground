// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UBotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Bind(class UHealthComponent* HealthComponent);

private:
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UProgressBar* PB_HpBar; // UMG에서 만든 progress바와 이름이 같아야 함

private:
	UFUNCTION()
	void UpdateHp(int32 Hp, int32 MaxHp);
};
