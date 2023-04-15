#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MovementInfo.generated.h"

/**
 * 
 */

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew )
class UMovementInfo : public UObject
{
	GENERATED_BODY()

private:
	friend class AProtagonistCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	FVector2D Dir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	bool OnJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess=true))
	TEnumAsByte<enum EMovementMode> CurrentMovementMode;

public:
	FORCEINLINE FVector2D GetDir() const { return Dir; }
};