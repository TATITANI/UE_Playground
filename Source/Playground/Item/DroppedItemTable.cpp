// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DroppedItemTable.h"

#include "ItemData.h"

TPair<UItemData*, FItemStatus> UDroppedItemTable::GetDroppedItemData()
{
	TPair<UItemData*, FItemStatus> ResultData;
	ResultData.Key = Table.Last().ItemData;
	ResultData.Value.Count = FMath::RandRange( Table.Last().MinCnt, Table.Last().MaxCnt);

	float AccumWeight = 0;
	for (auto Data : Table)
	{
		AccumWeight += Data.AppearanceWeight;
	}

	float TargetWeight = FMath::FRandRange(0, AccumWeight);
	for (auto Data : Table)
	{
		TargetWeight -= Data.AppearanceWeight;
		if (TargetWeight <= 0)
		{
			ResultData.Key = Data.ItemData;
			ResultData.Value.Count = FMath::RandRange( Data.MinCnt, Data.MaxCnt);
			break;
		}
	}

	return ResultData;
}
