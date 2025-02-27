#pragma once

#include "CoreMinimal.h"
#include "Data/WeaponStat.h"
#include "UObject/Object.h"
#include "Utils/UtilPlayground.h"
#include "CharacterCurrentInfo.generated.h"

/**
 * 
 */


DECLARE_DELEGATE_OneParam(FOnChangedCharacterCurrentInfo, FCharacterCurrentInfo);

USTRUCT(Atomic, BlueprintType)
struct PLAYGROUND_API FCharacterCurrentInfo
{
	GENERATED_BODY()

public:
	FCharacterCurrentInfo() : OnChangedCharacterCurrentInfo(MakeShared<FOnChangedCharacterCurrentInfo>())
	{
	};
	~FCharacterCurrentInfo();

private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=WEAPON, meta=(AllowPrivateAccess=true))
	EWeaponType CurrentWeaponType = EWeaponType::WEAPON_None;

	// public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess=true))
	FVector2D InputDir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Movement, meta=(AllowPrivateAccess=true))
	bool OnBeginJump = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess=true))
	bool OnClimbing = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Health, meta=(AllowPrivateAccess=true))
	bool OnHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess=true))
	TEnumAsByte<EMovementMode> CurrentMovementMode = EMovementMode::MOVE_None;

	TSharedPtr<FOnChangedCharacterCurrentInfo> OnChangedCharacterCurrentInfo;

public:
	FORCEINLINE FVector2D GetInputDir() const { return InputDir; }
	FORCEINLINE bool GetOnBeginJump() const { return OnBeginJump; }
	FORCEINLINE bool GetOnClimbing() const { return OnClimbing; }
	FORCEINLINE bool GetOnHit() const { return OnHit; }
	FORCEINLINE TEnumAsByte<EMovementMode> GetCurrentMovementMode() const { return CurrentMovementMode; }


	void SetInputDir(FVector2D _InputDir);
	void SetOnBeginJump(bool _OnBeginJump);
	void SetOnClimbing(bool _OnClimbing);
	void SetOnHit(bool _OnHit);
	void SetCurrentMovementMode(EMovementMode _MovementMode);

	void SetCurrentWeaponType(EWeaponType WeaponType) { CurrentWeaponType = WeaponType; }

	template <typename UserClass>
	void BindOnChangedCharacterCurrentInfo(UserClass* Object, void (UserClass::*InFunc)(FCharacterCurrentInfo))
	{
		if (Object && Object->IsValidLowLevel() && OnChangedCharacterCurrentInfo)
		{
			*OnChangedCharacterCurrentInfo = FOnChangedCharacterCurrentInfo::CreateUObject(Object, InFunc);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("invalid Delegate - BindOnChangedCharacterCurrentInfo"));
			if (OnChangedCharacterCurrentInfo)
			{
				OnChangedCharacterCurrentInfo->Unbind();
			}
		}
	}
};
