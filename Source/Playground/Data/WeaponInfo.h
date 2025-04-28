// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponInfo.generated.h"

/**
 *
 */

UENUM(BlueprintType)
namespace PGWeapon
{
	enum EWeaponType : uint8
	{
		SWORD,
		GUN,
		BOMB
		
	};

}

UCLASS(BlueprintType)
class PLAYGROUND_API UWeaponInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()
	using namespace PGWeapon;

	UPROPERTY()
	PGWeapon::EWeaponType WeaponType;


};
