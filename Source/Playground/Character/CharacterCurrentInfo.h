#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterCurrentInfo.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum EWeaponType
{
	WEAPON_None UMETA(DisplayName="None"),
	SWORD UMETA(displayDisplayName = "Sword"),
	GUN UMETA(displayDisplayName = "Gun"),
};

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class UCharacterCurrentInfo : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	FVector2D Dir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	bool OnJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	TEnumAsByte<enum EMovementMode> CurrentMovementMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=WEAPON)
	TEnumAsByte<enum EWeaponType> CurrentWeaponType = EWeaponType::WEAPON_None;

public:
	FORCEINLINE FVector2D GetDir() const { return Dir; }

	void SetCurrentWeaponType(EWeaponType WeaponType) { this->CurrentWeaponType = WeaponType;}
};
