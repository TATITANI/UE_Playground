// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGrabWall);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PLAYGROUND_API UClimbComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClimbComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	class AProtagonistCharacter* ProtagonistCharacter;
	class UProtagonistAnimInstance* AnimInstance;
	class UEnhancedInputLocalPlayerSubsystem* Subsystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* PreClimbingMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* GrabWallAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* ClimbMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ClimbAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ClimbJumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ClimbFallAction;


	UPROPERTY(EditDefaultsOnly, Category= Trace, meta=(AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> WallTraceChannel;

	UPROPERTY(EditDefaultsOnly, Category= Trace, meta=(AllowPrivateAccess = true))
	TEnumAsByte<ETraceTypeQuery> VisibilityChannel;

	UPROPERTY(EditDefaultsOnly, Category= Ani, meta=(AllowPrivateAccess = true))
	UAnimMontage* ClimbMontage;

	UPROPERTY(EditDefaultsOnly, Category= Jump, meta=(AllowPrivateAccess = true))
	UCurveFloat* JumpSpeedCurve;


	FDelegateHandle ClimbOverDelegateHandle;
	FOnGrabWall OnGrabWall;

	float CapsuleHalfHeight;
	float CapsuleRadius;
	float WalkableAngle;

	bool CanControlMoving = true;
	const float JumpDistance = 250;
	float RemainJumpDistance = 0;
	FVector LastJumpLoc;
	FVector2D JumpDir = FVector2D::Zero();
	
private:
	void Stop();
	void Move(const FInputActionValue& Value);
	void Climb(FVector2D InputDir, float Speed = 100.f);
	bool CanLand();

	FHitResult TraceClimbing(FVector StartLoc, float Distance = 400.0f, EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None);
	void ActiveClimbing(bool IsActive, bool IsStand = true);
	void GrabWall();
	void Jump();

	void PlayMontage(FName SectionName, float InPlayRate = 1.0f) const;

	void SetCanControlMoving(bool IsCan);
	FVector GetHeadLocation() const;

	bool CanClimbOver(FVector &ClimbOverLoc);

	void StartJump(FVector2D Dir, FName SectionName, float InPlayRate = 1.0f);
	void ProgressJump();
	
};
