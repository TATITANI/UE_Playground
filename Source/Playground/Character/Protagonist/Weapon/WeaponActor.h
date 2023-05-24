// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponActor.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	AWeaponActor();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void SetupInput();

protected:
	virtual void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent) PURE_VIRTUAL(AWeaponActor::BindInputActions,);
	virtual EWeaponType GetWeaponType() PURE_VIRTUAL(AWeaponActor::GetWeaponType, return WEAPON_None;);
	virtual void AttachEvent();

	UProtagonistAnimInstance *AnimInstance;

protected:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;

	// MappingContext Priority
	int32 InputPriority = 1;

	/** The Character holding this weapon*/
	AProtagonistCharacter* Character;

	UPROPERTY(EditDefaultsOnly, Category= Socket, meta=(AllowPrivateAccess=true))
	FName SocketName = "GripPoint_Sword";
	
	int32 Damage = 0;

public:
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AProtagonistCharacter* TargetCharacter);
};
