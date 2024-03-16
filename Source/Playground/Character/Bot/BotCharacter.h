// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BotCharacter.generated.h"

UENUM(BlueprintType)
namespace EBotState
{
	enum Type
	{
		Idle,
		Attacking,
		Attacked,
		KnockOut,
		StandUp,
	};
}


UCLASS()
class PLAYGROUND_API ABotCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABotCharacter();

protected:
	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void Init(class ABotGenerator* _Generator, FVector Loc);

	virtual void Attack();
	bool IsIdle();
	void KnockOut();

public:
	FSimpleMulticastDelegate OnAttackEnd;

	UPROPERTY(EditDefaultsOnly)
	class UWidgetComponent* HpWidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	class UHealthComponent* HealthComponent;
	
private:
	ABotGenerator* Generator;

	UPROPERTY()
	class UBotAnimInstance* AnimInstance;

	UPROPERTY()
	class ABotAIController* AIController;

	/**
	 * @brief 캐릭터 회전값을 기준으로 한 이동각도
	 */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	float CurrentVelocityAngle;
	
	UPROPERTY(VisibleAnywhere, Category="Attack")
	int32 AttackDamage;

	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	float AttackDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	float AttackRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	TEnumAsByte<ETraceTypeQuery> AttackTraceQuery;

	FTimerHandle KnockOutTimerHandle;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	float KnockOutDuration = 1;


	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TEnumAsByte<EBotState::Type> CurrentBotState = EBotState::Idle;

public:
	TEnumAsByte<EBotState::Type> GetCurrentState() const { return CurrentBotState; };
	void SetState(EBotState::Type BotState);

	/**
	 * @brief  현재 상태가 파라미터 상태와 같으면 idle 상태로 초기화.
	 * 다른 상태로 변경되었다면 현재 상태를 그대로 유지하는 목적. 
	 * @param ResetConditionState 
	 * @return 리셋 성공 
	 */
	bool ResetCurrentState(EBotState::Type ResetConditionState);
private:
	UFUNCTION()
	void BindUI();

	virtual void CheckAttack();

	UFUNCTION()
	void OnDeadCallback();

	UFUNCTION()
	void OnTakeDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
