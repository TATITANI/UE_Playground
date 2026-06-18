// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProtagonistMovementComponent.generated.h"

/**
 * 
 */

class FSavedMove_Protagonist : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public:
	bool bFixLocation = false;

	// 초기화
	virtual void Clear() override;

	// 움직임 상태 설정
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	// 움직임 데이터를 저장
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel,
	                        FNetworkPredictionData_Client_Character& ClientData) override;

	// 서버에 데이터 복원
	virtual void PrepMoveFor(ACharacter* Character) override;
};

class FNetworkPredictionData_Client_Protagonist : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_Protagonist(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class PLAYGROUND_API UProtagonistMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	UProtagonistMovementComponent();
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;


	UPROPERTY(ReplicatedUsing=OnRep_FixLocation)
	bool bFixLocation = false;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_FixLocation();
	
public:
	UFUNCTION(Server, Reliable)
	void ServerFixLocation(bool InFixLocation);
};
