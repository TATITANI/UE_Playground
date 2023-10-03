// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemInventory.generated.h"

/**
 * 
 */
UCLASS()
class UItemInventory : public UObject
{
	GENERATED_BODY()

public:
	UItemInventory();

private:
	UPROPERTY(VisibleAnywhere)
	TMap<UItemData*, FItemStatus> Table;

	UPROPERTY(VisibleAnywhere)
	int32 SlotCnt = 40;

	// 슬롯 사용가능 확인 빠르게 하려고 별도 저장
	TArray<UItemData*> SlotTable;

public:
	void Init(TMap<UItemData*, FItemStatus> _Table) { this->Table = _Table; };
	void AddItem(UItemData* ItemData, int32 Cnt);
	void SwapSlotID(int32 SlotID1, int32 SlotID2);

	TOptional<FItemStatus> GetItem(UItemData* ItemData);

	UFUNCTION(BlueprintCallable)
	TMap<UItemData*, FItemStatus> GetTable();
};
