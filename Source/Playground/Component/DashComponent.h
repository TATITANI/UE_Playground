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
	class UProtagonistAnimInstance *AnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Ani", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditDefaultsOnly, Category="dash")
	UCurveFloat* DashPowerCurve;

	UPROPERTY(EditDefaultsOnly, Category="dash",  meta=(AllowPrivateAccess))
	float DashDistance = 5000.0f;
	
	float RemainDistance = 0;

	UPROPERTY(EditDefaultsOnly, Category="dash",  meta=(AllowPrivateAccess))
	float CoolTime = 2.f;
	
	bool IsCooltime = false;
	FTimerHandle CooltimeTimerHandle;
private:
	UFUNCTION()
	void ProgressDash(float& dt);

	UFUNCTION()
	void StartDash(const FInputActionValue& Value);
};
