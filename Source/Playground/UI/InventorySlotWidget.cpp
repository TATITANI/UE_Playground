// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventorySlotWidget.h"

#include "ItemSlotDrag.h"
#include "InventoryWidget.h"
#include "ItemPreviewSlot.h"
#include "MyGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/ItemData.h"
#include "Kismet/GameplayStatics.h"


void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UInventorySlotWidget::Init(UInventoryWidget* _InventoryWidget, int32 _SlotID)
{
	this->InventoryWidget = _InventoryWidget;
	this->SlotID = _SlotID;
	Btn->OnClicked.AddUniqueDynamic(this, &UInventorySlotWidget::UInventorySlotWidget::OnClick);
	
}

void UInventorySlotWidget::UpdateUI(UItemData* _ItemData, int32 _Cnt)
{
	ItemData = _ItemData;
	Cnt = _Cnt;

	const FText CntText = Cnt == 0 ? FText::FromString(" ") : FText::FromString(FString::Printf(TEXT("X %d"), Cnt));
	Txt_Cnt->SetText(CntText);

	if (ItemData != nullptr)
	{
		Img_Icon->SetBrushFromTexture(ItemData->ItemIcon);
	}

	Img_Icon->SetVisibility(ItemData != nullptr ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Txt_Cnt->SetVisibility(ItemData != nullptr ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	Btn->SetVisibility(ItemData != nullptr ? ESlateVisibility::Visible : ESlateVisibility::HitTestInvisible);
	
	UpdateTooltip();
}


void UInventorySlotWidget::OnClick()
{
}



FReply UInventorySlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (ItemData != nullptr)
	{
		if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
		{
			auto Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
			return Reply.NativeReply;
		}
	}
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (OutOperation == nullptr)
	{
		if (DragVisualClass != nullptr)
		{
			UItemPreviewSlot* VisualSlot = CreateWidget<UItemPreviewSlot>
				(UGameplayStatics::GetPlayerController(GetWorld(), 0), DragVisualClass);

			VisualSlot->Img->SetBrushFromTexture(ItemData->ItemIcon);

			auto DraggableSlot = NewObject<UItemSlotDrag>();
			DraggableSlot->DefaultDragVisual = VisualSlot;
			DraggableSlot->SlotID = this->SlotID;
			
			OutOperation = DraggableSlot;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Drag : Drag again"));
	}
}


bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	auto Oper = Cast<UItemSlotDrag>(InOperation);
	if(SlotID != Oper->SlotID)
	{
		SwapSlot(Oper->SlotID);
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}


void UInventorySlotWidget::SwapSlot(int32 TargetSlotID)
{
	UItemData* _ItemData = this->ItemData;
	int32 _Cnt = this->Cnt;
	UInventorySlotWidget* SwapSlot = InventoryWidget->GetItemSlot(TargetSlotID);
	UpdateUI(SwapSlot->ItemData, SwapSlot->Cnt);
	SwapSlot->UpdateUI(_ItemData, _Cnt);
	
	auto ItemInventory = Cast<UMyGameInstance>(GetGameInstance())->ItemInventory;
	ItemInventory->SwapSlotID(this->SlotID, SwapSlot->SlotID);

}