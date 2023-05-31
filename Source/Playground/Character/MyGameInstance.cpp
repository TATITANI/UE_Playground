// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	WeaponInventory = MakeShared<FWeaponInventory>();
}

TOptional<FCharacterData> UMyGameInstance::GetCharacterData(ECharacterStatType StatType, FName RowName)
{
	UDataTable* DataTable = *StatMap.Find(StatType);
	ensure(DataTable != nullptr);
	const FCharacterData* Data = DataTable->FindRow<FCharacterData>(RowName,TEXT(""));

	if (Data != nullptr)
		return *Data;

	return NullOpt;
}
