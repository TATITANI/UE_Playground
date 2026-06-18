#include "CharacterCurrentInfo.h"


FCharacterCurrentInfo::~FCharacterCurrentInfo()
{
	OnChangedCharacterCurrentInfo.Reset();
}

void FCharacterCurrentInfo::SetInputDir(FVector2D _InputDir)
{
	InputDir = _InputDir;
	OnChangedCharacterCurrentInfo->ExecuteIfBound(*this);
}

void FCharacterCurrentInfo::SetOnBeginJump(bool _OnBeginJump)
{
	OnBeginJump = _OnBeginJump;
	OnChangedCharacterCurrentInfo->ExecuteIfBound(*this);

}

void FCharacterCurrentInfo::SetOnClimbing(bool _OnClimbing)
{
	OnClimbing = _OnClimbing;
	OnChangedCharacterCurrentInfo->ExecuteIfBound(*this);
}

void FCharacterCurrentInfo::SetOnHit(bool _OnHit)
{
	OnHit = _OnHit;
	OnChangedCharacterCurrentInfo->ExecuteIfBound(*this);
}

void FCharacterCurrentInfo::SetCurrentMovementMode(EMovementMode _MovementMode)
{
	CurrentMovementMode = _MovementMode;
	OnChangedCharacterCurrentInfo->ExecuteIfBound(*this);
}
