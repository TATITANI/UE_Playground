// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInventory.h"

UItemInventory::UItemInventory()
{
	SlotTable.SetNum(SlotCnt);
}


void UItemInventory::AddItem(UItemData* ItemData, int32 Cnt)
{
	if (Table.Contains(ItemData) == false)
	{
		int32 SlotID = 0;
		for (; SlotID < SlotTable.Num(); SlotID++)
		{
			if (SlotTable[SlotID] == nullptr)
			{
				break;
			}
		}
		if (SlotID >= SlotTable.Num())
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to add more items because inventory is full"));
			return;
		}

		FItemStatus ItemStatus;
		ItemStatus.Count = 0;
		ItemStatus.SlotID = SlotID;
		Table.Add(ItemData, ItemStatus);
		SlotTable[SlotID] = ItemData;
	}

	Table[ItemData].Count += Cnt;
}



void UItemInventory::SwapSlotID(int32 SlotID1, int32 SlotID2)
{
	const UItemData* Data1 = SlotTable[SlotID1];	
	const UItemData* Data2 = SlotTable[SlotID2];
	SlotTable.Swap(SlotID1, SlotID2);
	if(Data1 != nullptr)
		Table[Data1].SlotID = SlotID2;
	if(Data2 != nullptr)
		Table[Data2].SlotID = SlotID1;
	
}

TOptional<FItemStatus> UItemInventory::GetItem(UItemData* ItemData)
{
	if (Table.Contains(ItemData))
	{
		return Table[ItemData];
	}

	return NullOpt;
}

TMap<UItemData*, FItemStatus> UItemInventory::GetTable()
{
	return Table;
}
