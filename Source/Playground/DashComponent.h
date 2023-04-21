// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	AActor* Owner;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, Category=DASH)
	UCurveFloat* DashPowerCurve;

	FTimerHandle DashTimerHandle;
	const float DashPower = 2000.f;
	const float DashCooltime = 3.f;
	bool CanDash = true;

	float DashRemainTime = 0;
	float DashMaxTime;

private:
	UFUNCTION()
	void ProgressDash(float& dt);

	UFUNCTION()
	void StartDash(const FInputActionValue& Value);
};
