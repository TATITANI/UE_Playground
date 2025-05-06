// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
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
	virtual void Tick(float DeltaSeconds) override;
	bool IsBindInputAction = false;

private:
	void SetupInput();
	void AddInputMappingContext(const APlayerController* PlayerController);
	void RemoveInputMappingContext();

protected:

	UPROPERTY(EditDefaultsOnly, meta= (AllowPrivateAccess=true))
	UWeaponInfo* WeaponInfo;

	UPROPERTY()
	TObjectPtr<UProtagonistAnimInstance> ProtagonistAnimInstance;

	UPROPERTY(ReplicatedUsing = OnRep_Protagonist)
	AProtagonistCharacter* OwnerProtagonist;

	UFUNCTION()
	void OnRep_Protagonist();
	
	class UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UPROPERTY()
	UMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;

	// MappingContext Priority
	int32 InputPriority = 1;

	UPROPERTY(EditDefaultsOnly, Category= Socket, meta=(AllowPrivateAccess=true))
	FName SocketName = "GripPoint_Sword";

	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* AttackInputAction;

	bool IsCharging = false;
	bool IsAttack = false;
	int32 ReusableCnt;

	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess))
	int32 ReusableMaxCnt = 0;
	
	FTimerHandle RefillTimerHandle;

	float Damage;

	float CoolTime;
	double RefillTime;

private:
	void BindInputActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnAttackInputStarted();
	void OnAttackInputTriggered();
	void OnAttackInputCompleted();

	void AttackTriggerIfPossible(ETriggerEvent TriggerEvent);

	void CooldownIfPossible(ETriggerEvent TriggerEvent);


	void AttachToProtagonist();

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BindInputActionsImpl(UEnhancedInputComponent* EnhancedInputComponent){}
	virtual void AttackInputStarted()	{	};
	virtual void AttackInputTrigger(){	};
	virtual void AttackInputCompleted()	{	};

	virtual ETriggerEvent GetAttackTriggerEvent() PURE_VIRTUAL(AWeaponActor::GetAttackTriggerEvent, return ETriggerEvent::None;);
	virtual void OnRefill();

	
public:

	UFUNCTION()
	void OnObtained(AProtagonistCharacter* InProtagonist);


	//virtual EWeaponType GetWeaponType() PURE_VIRTUAL(AWeaponActor::GetWeaponType, return EWeaponType::NONE;);
	UWeaponInfo* GetWeaponInfo() const { return WeaponInfo; }
	EWeaponType GetWeaponType() const { return WeaponInfo->WeaponType; }

	virtual void Equip(AProtagonistCharacter* TargetCharacter);
	virtual void UnEquip();

	FName GetSocketName() const { return SocketName; }
};
