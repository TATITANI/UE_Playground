// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Hp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage;

};
