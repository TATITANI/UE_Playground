// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventorySlotWidget.h"

#include "Components/Border.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/ItemData.h"

void UInventorySlotWidget::UpdateData(UItemData* _ItemData, int32 _Cnt)
{
	ItemData = _ItemData;
	Cnt = _Cnt;

	Img_Icon->SetBrushFromTexture(ItemData->ItemIcon);
	Txt_Cnt->SetText(FText::FromString(FString::Printf(TEXT("%d"), Cnt)));
}

void UInventorySlotWidget::SetActive(bool bActive)
{
	const ESlateVisibility SlateVisibility = bActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	Img_Icon->SetVisibility(SlateVisibility);
	Txt_Cnt->SetVisibility(SlateVisibility);

	if (!bActive)
	{
		ItemData = nullptr;
	}
}
