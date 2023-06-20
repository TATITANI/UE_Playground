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
	BOMB UMETA(displayDisplayName = "Bomb"),
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class UCharacterCurrentInfo : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=WEAPON, meta=(AllowPrivateAccess=true))
	TEnumAsByte<EWeaponType> CurrentWeaponType = WEAPON_None;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	FVector2D InputDir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	bool OnJump = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	bool OnClimbing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	TEnumAsByte<EMovementMode> CurrentMovementMode;


public:
	FORCEINLINE FVector2D GetDir() const { return InputDir; }

	void SetCurrentWeaponType(EWeaponType WeaponType) { this->CurrentWeaponType = WeaponType; }
};
