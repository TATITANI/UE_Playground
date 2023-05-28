// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponPanelWidget.h"
#include "Materials/MaterialInstance.h"
#include "Components/Image.h"

void UWeaponPanelWidget::Init(UTexture2D* Tex, EWeaponType _WeaponType)
{
	Img_Weapon->SetBrushFromTexture(Tex);
	Img_Weapon->SetVisibility(ESlateVisibility::Visible);
	this->WeaponType = _WeaponType;

	// brush 메터리얼을 dynamic으로 설정
	UMaterialInterface* MaterialInterface = Cast<UMaterialInterface>(Img_Panel->Brush.GetResourceObject());
	MaterialInstanceDynamic = UMaterialInstanceDynamic::Create(MaterialInterface, nullptr);
	MaterialInstanceDynamic->AddToRoot(); // GC 삭제 방지
	Img_Panel->SetBrushFromMaterial(MaterialInstanceDynamic);
	
}

void UWeaponPanelWidget::ActiveUseEffect(bool bActive) const
{
	MaterialInstanceDynamic->SetScalarParameterValue(TEXT("IsActive"), bActive);
}
