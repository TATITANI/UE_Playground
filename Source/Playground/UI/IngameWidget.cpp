// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameWidget.h"

#include "WeaponPanelWidget.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Component/StatComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UIngameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ProtagonistCharacter = Cast<AProtagonistCharacter>(PlayerPawn);
	ensure(ProtagonistCharacter!= nullptr);
	ProtagonistCharacter->OnChangeWeapon.AddUObject(this, &UIngameWidget::ChangeCurrentWeapon);
	ProtagonistCharacter->OnObtainWeapon.AddUObject(this, &UIngameWidget::AddWeapon);
	UE_LOG(LogTemp, Log, TEXT(" UIngameWidget::NativeOnInitialized"));

	UStatComponent* StatComponent = Cast<UStatComponent>(ProtagonistCharacter->FindComponentByClass(UStatComponent::StaticClass()));
	ensure(StatComponent!= nullptr);
	StatComponent->OnHpChanged.AddUObject(this, &UIngameWidget::UpdateHp);
}

void UIngameWidget::UpdateHp(int32 Hp, int32 MaxHp) const
{
	if (ensure(PB_HpBar))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero %d"));
		const float ratio = static_cast<float>(Hp) / MaxHp;
		PB_HpBar->SetPercent(ratio);
	}
}

void UIngameWidget::ChangeCurrentWeapon(AWeaponActor* WeaponActor) const
{
	const auto WeaponType = WeaponActor->GetWeaponType();

	for (int8 i = 0; i < NewWeaponIndex; i++)
	{
		const UWeaponPanelWidget* WeaponPanelWidget = Cast<UWeaponPanelWidget>(HB_Weapon->GetChildAt(i));
		const bool IsUse = WeaponType == WeaponPanelWidget->GetWeaponType();
		WeaponPanelWidget->ActiveUseEffect(IsUse);
	}
}

void UIngameWidget::AddWeapon(AWeaponActor* WeaponActor)
{
	const auto WeaponType = WeaponActor->GetWeaponType();
	ensure(WeaponImageTable.Contains(WeaponType));
	if (WeaponImageTable.Contains(WeaponType) == false)
		return;

	UTexture2D* Texture = WeaponImageTable[WeaponType];
	UWeaponPanelWidget* WeaponPanelWidget = Cast<UWeaponPanelWidget>(HB_Weapon->GetChildAt(NewWeaponIndex));
	WeaponPanelWidget->Init(Texture, WeaponType);
	NewWeaponIndex++;
}
