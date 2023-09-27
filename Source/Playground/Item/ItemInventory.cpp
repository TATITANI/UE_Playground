// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemInventory.h"

UItemInventory::UItemInventory()
{
}


void UItemInventory::AddItem(UItemData* ItemData, int32 Cnt)
{
	if(Table.Contains(ItemData) == false)
	{
		FItemStatus ItemStatus;
		ItemStatus.Count = 0;
		Table.Add(ItemData,ItemStatus);
	}
	
	Table[ItemData].Count += Cnt;
}

TOptional<FItemStatus> UItemInventory::GetItem(UItemData* ItemData)
{
	if(Table.Contains(ItemData))
	{
		return Table[ItemData];
	}

	return NullOpt;
}

TMap<UItemData*, FItemStatus> UItemInventory::GetTable()
{
	return Table;
}
