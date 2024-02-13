// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatingDamage.generated.h"

/**
 * 
 */
UCLASS()
class PLAYGROUND_API AFloatingDamage : public AActor
{
	GENERATED_BODY()
public:
	AFloatingDamage();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	class UWidgetComponent* WidgetComponent;

	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta=(AllowPrivateAccess))
	FVector StartLoc;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, meta=(AllowPrivateAccess))
	FVector EndLoc;

	UPROPERTY(EditDefaultsOnly)
	FVector2f EndLocMin = FVector2f(10,10);

	UPROPERTY(EditDefaultsOnly)
	FVector2f EndLocMax = FVector2f(30,30);
	
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* MovingCurve;

	UPROPERTY(BlueprintSetter=SetMovingDuration, VisibleAnywhere)
	float MovingDuration = 1;

	float ElapsedTime = 0;
	
protected:
	UFUNCTION(BlueprintSetter)
	void SetMovingDuration(float _MovingDuration);
	
public:

	void Init(int32 Damage, FVector Loc, FRotator Rot);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageTxt(const int32 &Damage);
	
};

