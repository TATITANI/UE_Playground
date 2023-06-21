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
	class AProtagonistCharacter *ProtagonistCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, Category=DASH)
	UCurveFloat* DashPowerCurve;

	const float DashPower = 2000.0f;
	const float DashDistanceSquared = 5000.0f;
	float RemainDistance = 0;
	
	bool IsCooltime = false;
	FTimerHandle CooltimeTimerHandle;
	const float CoolTime = 2.f;

private:
	UFUNCTION()
	void ProgressDash(float& dt);

	UFUNCTION()
	void StartDash(const FInputActionValue& Value);
};
