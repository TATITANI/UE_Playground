// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
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

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn;

private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	TSubclassOf<class UItemPreviewSlot> DragVisualClass;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	class UItemData* ItemData = nullptr;
	
	class UInventoryWidget* InventoryWidget;
	int32 Cnt;
	int32 SlotID;

	void SwapSlot(int32 TargetSlotID);

protected:
	
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTooltip();
	
public:
	virtual void NativeConstruct() override;
	void Init(class UInventoryWidget* _InventoryWidget, int32 _SlotID);
	void UpdateUI(UItemData* _ItemData, int32 _Cnt);

	UFUNCTION(BlueprintCallable)
	void OnClick();


	UItemData* GetItemData() const { return this->ItemData; }
};
