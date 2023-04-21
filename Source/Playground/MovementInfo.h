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
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	FVector2D Dir = FVector2d::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Movement)
	bool OnJump = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	TEnumAsByte<enum EMovementMode> CurrentMovementMode;

public:
	FORCEINLINE FVector2D GetDir() const { return Dir; }
};