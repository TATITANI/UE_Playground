// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "WeaponActor.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "WeaponActor.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API AWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	/** Sets default values for this component's properties */
	AWeaponActor();

protected:
	virtual void BeginPlay() override;

	bool IsBindInputAction = false;

private:
	void SetupInput();
	void AddInputMappingContext(const APlayerController* PlayerController);
	void RemoveInputMappingContext();

protected:
	UProtagonistAnimInstance* AnimInstance;
	class UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UMeshComponent* MeshComponent;

	float Damage;

	float CoolTime;
	double RefillTime;

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

	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AttackInputAction;


	bool IsCharging = false;
	bool IsAttack = false;
	virtual void OnRefill();

	FTimerHandle RefillTimerHandle;

private:
	void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnAttackStarted();
	void OnAttackTriggered();
	void OnAttackCompleted();

	void CooldownIfPossible(ETriggerEvent TriggerEvent);
protected:
	virtual void AttackStart()	{	};
	virtual void AttackTrigger(){	};
	virtual void AttackFinish()	{	};

	virtual ETriggerEvent GetCooldownOccurEvent() PURE_VIRTUAL(AWeaponActor::GetCooldownOccurEvent, return ETriggerEvent::None;);
	virtual bool CheckCooldown();
	

public:
	virtual EWeaponType GetWeaponType() PURE_VIRTUAL(AWeaponActor::GetWeaponType, return EWeaponType::WEAPON_None;);

	virtual void Use(AProtagonistCharacter* TargetCharacter);
	virtual void UnUse();

	FName GetSocketName() const { return SocketName; }
};
