// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameWidget.h"

#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Component/StatComponent.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UIngameWidget::UpdateHp(int32 Hp, int32 MaxHp) const
{
	if (ensure(PB_HpBar))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero %d"));
		const float ratio = static_cast<float>(Hp) / MaxHp;
		PB_HpBar->SetPercent(ratio);
	}
}

void UIngameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ProtagonistCharacter = Cast<AProtagonistCharacter>(PlayerPawn);
	ensure(ProtagonistCharacter!= nullptr);

	UStatComponent* StatComponent = Cast<UStatComponent>(ProtagonistCharacter->FindComponentByClass(UStatComponent::StaticClass()));
	ensure(StatComponent!= nullptr);
	StatComponent->OnHpChanged.AddUObject(this, &UIngameWidget::UpdateHp);

}
