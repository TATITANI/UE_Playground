// Fill out your copyright notice in the Description page of Project Settings.


#include "BotWidget.h"
#include "Component/HealthComponent.h"
#include "Components/ProgressBar.h"

void UBotWidget::Bind(UHealthComponent* HealthComponent)
{
	if (ensure(HealthComponent != nullptr))
	{
		HealthComponent->OnHpChanged.AddUObject(this, &UBotWidget::UpdateHp);
	}
}


void UBotWidget::UpdateHp(int32 Hp,  int32 DeltadHp, int32 MaxHp)
{
	if (ensure(PB_HpBar))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero %d"));
		const float ratio = static_cast<float>(Hp) / MaxHp;
		PB_HpBar->SetPercent(ratio);
	}
}
