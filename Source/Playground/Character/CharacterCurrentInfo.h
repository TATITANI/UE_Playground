#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponStat.h"
#include "UObject/Object.h"
#include "CharacterCurrentInfo.generated.h"

/**
 * 
 */



USTRUCT(Atomic, BlueprintType)
struct PLAYGROUND_API FCharacterCurrentInfo 
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=WEAPON, meta=(AllowPrivateAccess=true))
	EWeaponType CurrentWeaponType = EWeaponType::WEAPON_None;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	FVector2D InputDir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Movement)
	bool OnBeginJump = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	bool OnClimbing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	TEnumAsByte<EMovementMode> CurrentMovementMode;


public:
	// UFUNCTION()
	FORCEINLINE FVector2D GetDir() const { return InputDir; }

	// UFUNCTION()
	void SetCurrentWeaponType(EWeaponType WeaponType) { CurrentWeaponType = WeaponType; }
};
