// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameWidget.h"

#include "WeaponSlotWidget.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Component/CharacterWeaponComponent.h"
#include "Component/HealthComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void UIngameWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ProtagonistCharacter = Cast<AProtagonistCharacter>(PlayerPawn);
	ensure(ProtagonistCharacter!= nullptr);
	ProtagonistCharacter->WeaponComponent->OnChangeWeapon.AddUObject(this, &UIngameWidget::ChangeCurrentWeapon);
	ProtagonistCharacter->WeaponComponent->OnObtainWeapon.AddUObject(this, &UIngameWidget::AddWeapon);
	ProtagonistCharacter->WeaponComponent->OnCooldownWeapon.AddUObject(this,&UIngameWidget::Cooldown);
	ProtagonistCharacter->WeaponComponent->OnUseWeapon.AddUObject(this, &UIngameWidget::UseWeapon);
	
	UE_LOG(LogTemp, Log, TEXT(" UIngameWidget::NativeOnInitialized"));

	UHealthComponent* HealthComponent = Cast<UHealthComponent>(ProtagonistCharacter->FindComponentByClass(UHealthComponent::StaticClass()));
	ensure(HealthComponent!= nullptr);
	HealthComponent->OnHpChanged.AddUObject(this, &UIngameWidget::UpdateHp);

	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Img_HP_Gauge->Brush.GetResourceObject());
	MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, nullptr);
	MaterialInstanceDynamic->AddToRoot(); // GC 삭제 방지
	Img_HP_Gauge->SetBrushFromMaterial(MaterialInstanceDynamic);
}

void UIngameWidget::UpdateHp(int32 Hp, int32 MaxHp) const
{
	if (ensure(Img_HP_Gauge))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero !"));
		const float HpRatio = static_cast<float>(Hp) / MaxHp;
		MaterialInstanceDynamic->SetScalarParameterValue(TEXT("FillAmount"), HpRatio);
	}
}

void UIngameWidget::ChangeCurrentWeapon(AWeaponActor* WeaponActor)
{
	const auto WeaponType = WeaponActor->GetWeaponType();

	if (CurrentWeaponSlot != nullptr)
	{
		CurrentWeaponSlot->ActiveUseEffect(false);
	}
	auto WeaponSlots = HB_Weapon->GetAllChildren();
	CurrentWeaponSlot = Cast<UWeaponSlotWidget>(*WeaponSlots.FindByPredicate([&](UWidget* SlotWidget)
	{
		return Cast<UWeaponSlotWidget>(SlotWidget)-> GetWeaponType() == WeaponType;
	}));

	ensureMsgf(CurrentWeaponSlot != nullptr, TEXT("changed weapon slot not exist"));
	CurrentWeaponSlot->ActiveUseEffect(true);
}

void UIngameWidget::AddWeapon(AWeaponActor* WeaponActor)
{
	const auto WeaponType = WeaponActor->GetWeaponType();
	ensure(WeaponImageTable.Contains(WeaponType));
	if (WeaponImageTable.Contains(WeaponType) == false)
		return;

	UTexture2D* Texture = WeaponImageTable[WeaponType];
	UWeaponSlotWidget* WeaponSlotWidget = Cast<UWeaponSlotWidget>(HB_Weapon->GetChildAt(NewWeaponIndex));
	WeaponSlotWidget->AssignWeapon(Texture, WeaponType);
	NewWeaponIndex++;
}

EWeaponType UIngameWidget::GetSlotWeaponType(int8 SlotID)
{
	const auto WeaponSlotWidget = Cast<UWeaponSlotWidget>(HB_Weapon->GetChildAt(SlotID));
	if (WeaponSlotWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("weapon slot not found"));
		return EWeaponType::WEAPON_None;
	}
	return WeaponSlotWidget->GetWeaponType();
}

void UIngameWidget::Cooldown(double BeginSeconds, double EndSeconds)
{
	CurrentWeaponSlot->ActiveCooldown(BeginSeconds, EndSeconds);
}

void UIngameWidget::UseWeapon(int32 RemainCnt, int32 MaxCnt)
{
	CurrentWeaponSlot->SetAvailableCnt(1-static_cast<float>(RemainCnt) / MaxCnt);
	
}
