// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/CharacterCurrentInfo.h"
#include "WeaponSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UImage* Img_Panel;

	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UImage* Img_Weapon;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType = EWeaponType::WEAPON_None;

	UMaterialInstanceDynamic* MaterialInstanceDynamic;

public:
	void Init(UTexture2D* Tex, EWeaponType _WeaponType);
	
	EWeaponType GetWeaponType() const { return this->WeaponType; }
	void ActiveUseEffect(bool bActive) const;
};
