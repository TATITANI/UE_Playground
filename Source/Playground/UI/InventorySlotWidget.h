// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UImage* Img_Icon;

	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UTextBlock* Txt_Cnt;

	UPROPERTY(meta=(AllowPrivateAccess, BindWidget))
	class UBorder* Border_Cnt;

	class UItemData *ItemData  = nullptr;
	int32 Cnt;

	
public:
	void UpdateData(UItemData *_ItemData, int32 _Cnt);
	void SetActive(bool bActive);

	UPROPERTY(meta=( BindWidget))
	class UCheckBox *CheckBox;

};
