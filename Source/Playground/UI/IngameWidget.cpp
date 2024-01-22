// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameWidget.h"

#include "MyGameInstance.h"
#include "PlaygroundGameMode.h"
#include "WeaponSlotWidget.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Character/Protagonist/Weapon/WeaponActor.h"
#include "Component/CharacterWeaponComponent.h"
#include "Component/HealthComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/UtilPlayground.h"

void UIngameWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// UtilPlayground::PrintLog(TEXT("UIngameWidget::NativeConstruct"));

	const auto PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	ProtagonistCharacter = Cast<AProtagonistCharacter>(PlayerPawn);
	ensure(ProtagonistCharacter!= nullptr);
	ProtagonistCharacter->WeaponComponent->OnChangeWeapon.AddUObject(this, &UIngameWidget::ChangeCurrentWeapon);
	ProtagonistCharacter->WeaponComponent->OnCooldownWeapon.AddUObject(this, &UIngameWidget::Cooldown);
	ProtagonistCharacter->WeaponComponent->OnUseWeapon.AddUObject(this, &UIngameWidget::UseWeapon);
	ProtagonistCharacter->HealthComponent->OnHpChanged.AddLambda([this](int32 HP, int32 Delta, int32 MaxHP)
	{
		if(Delta < 0)
		{
			PlayHitAnimation();
		}
	});
	
	const auto WeaponInventory = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->WeaponInventory;
	ensure(WeaponInventory != nullptr);
	WeaponInventory->OnAddWeapon.AddUObject(this, &UIngameWidget::AddWeapon);
	auto Weapons = WeaponInventory->GetWeapons();
	for(const auto Weapon: Weapons)
	{
		AddWeapon(Weapon);
	}


	UHealthComponent* HealthComponent = Cast<UHealthComponent>(ProtagonistCharacter->FindComponentByClass(UHealthComponent::StaticClass()));
	ensure(HealthComponent!= nullptr);
	HealthComponent->OnHpChanged.AddUObject(this, &UIngameWidget::UpdateHp);

	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Img_HP_Gauge->Brush.GetResourceObject());
	HPMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, nullptr);
	HPMaterialInstanceDynamic->AddToRoot(); // GC 삭제 방지
	Img_HP_Gauge->SetBrushFromMaterial(HPMaterialInstanceDynamic);

	
}


void UIngameWidget::UpdateHp(int32 CurrentHp, int32 DeltaHp, int32 MaxHp) const
{
	if (ensure(Img_HP_Gauge))
	{
		ensureMsgf(MaxHp!=0, TEXT("MaxHp is zero !"));
		const float HpRatio = static_cast<float>(CurrentHp) / MaxHp;
		HPMaterialInstanceDynamic->SetScalarParameterValue(TEXT("FillAmount"), HpRatio);
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
		return Cast<UWeaponSlotWidget>(SlotWidget)->GetWeaponType() == WeaponType;
	}));

	ensureMsgf(CurrentWeaponSlot != nullptr, TEXT("changed weapon slot not exist"));
	CurrentWeaponSlot->ActiveUseEffect(true);
}

void UIngameWidget::AddWeapon(AWeaponActor* WeaponActor)
{
	UtilPlayground::PrintLog(TEXT("AddWeapon"));
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
	CurrentWeaponSlot->SetAvailableCnt(1 - static_cast<float>(RemainCnt) / MaxCnt);
}
