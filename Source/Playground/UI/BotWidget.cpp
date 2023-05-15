// Fill out your copyright notice in the Description page of Project Settings.


#include "BotWidget.h"
#include "Component/StatComponent.h"
#include "Components/ProgressBar.h"

void UBotWidget::Bind(UStatComponent* StatComponent)
{
	if (ensure(StatComponent != nullptr))
	{
		StatComponent->OnHpChanged.AddUObject(this, &UBotWidget::UpdateHp);
	}
}


void UBotWidget::UpdateHp(int32 Hp, int32 MaxHp)
{
	if (ensure(PB_HpBar))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero %d"));
		const float ratio = static_cast<float>(Hp) / MaxHp;
		PB_HpBar->SetPercent(ratio);
	}
}
