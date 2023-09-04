// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterStat.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChanged, int32 /*Hp*/ , int32 /*maxHP*/);
DECLARE_MULTICAST_DELEGATE(FOnDead);
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void PostInitProperties() override;
	virtual void InitializeComponent() override;

private:
	UPROPERTY(EditAnywhere, Category=Stat, meta=(AllowPrivateAccess=true))
	ECharacterStatType StatType;

	int32 MaxHp = 0;
	int32 CurrentHp = 0;

private:
	UFUNCTION()
	void HandleTakenDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                       class AController* InstigatedBy, AActor* DamageCauser);

public:
	FOnHpChanged OnHpChanged;
	FOnDead OnDead;
	
	void Init(int32 _MaxHp);
	void Reset();
	int32 GetCurrentHP() const { return CurrentHp; }
	int32 GetMaxHP() const { return MaxHp; }
};
