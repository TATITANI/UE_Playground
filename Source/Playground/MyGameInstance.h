// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterData.h"
#include "Engine/GameInstance.h"
#include "Inventory/WeaponInventory.h"
#include "MyGameInstance.generated.h"

enum class ECharacterStatType : uint8;

UCLASS()
class PLAYGROUND_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	TMap<ECharacterStatType, UDataTable*> StatMap;

public:
	UMyGameInstance();
	// todo : 캐릭터 데이터 매니저로 빼기
	TOptional<FCharacterData> GetCharacterData(ECharacterStatType StatType, FName RowName);

	TSharedPtr<FWeaponInventory> WeaponInventory;
};
