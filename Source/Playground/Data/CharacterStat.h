// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterStat.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatType : uint8
{
	Protagonist UMETA(DisplayName="Protagonist") ,
	Bot UMETA(displayDisplayName = "Bot"),
};

USTRUCT(Atomic, BlueprintType)
struct FProtagonistStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp = 0;


};

USTRUCT(Atomic, BlueprintType)
struct FBotStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Damage = 0;

};





