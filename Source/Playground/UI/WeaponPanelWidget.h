// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponPanelWidget.generated.h"

enum EWeaponType : int;
/**
 * 
 */
UCLASS()
class PLAYGROUND_API UWeaponPanelWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UImage* Img_Panel;

	UPROPERTY(meta=(BindWidget, AllowPrivateAccess = true))
	class UImage* Img_Weapon;

	EWeaponType WeaponType;

	UMaterialInstanceDynamic* MaterialInstanceDynamic;

public:
	void Init(UTexture2D* Tex, EWeaponType _WeaponType);
	
	EWeaponType GetWeaponType() const { return this->WeaponType; }
	void ActiveUseEffect(bool bActive) const;
};
