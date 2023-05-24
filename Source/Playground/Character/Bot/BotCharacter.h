// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BotCharacter.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnAttackEnd);

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


public:
	virtual void Attack();

	TSharedPtr<FOnAttackEnd> OnAttackEnd;


private:
	UPROPERTY()
	class UBotAnimInstance* AnimInstance;
	
	UPROPERTY()
	class UStatComponent* StatComponent;
	

	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	float attackDistance = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Attack", meta=(AllowPrivateAccess=true))
	float attackRadius = 50.f;

private:
	UFUNCTION()
	void BindUI();
	
	virtual void CheckAttack();
};
