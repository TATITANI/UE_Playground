// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbComponent.h"

#include "CharacterWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Character/Protagonist/ProtagonistCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/UtilPlayground.h"

// Sets default values for this component's properties
UClimbComponent::UClimbComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ClimbFallAction = UtilPlayground::LoadAsset<
		UInputAction>("/Script/EnhancedInput.InputAction'/Game/Data/Input/Actions/IA_ClimbFall.IA_ClimbFall'");

	ClimbMontage = UtilPlayground::LoadAsset<UAnimMontage>("/Script/Engine.AnimMontage'/Game/ArtResources/Mixamo/Protagonist/AM_Climb.AM_Climb'");
	JumpSpeedCurve = UtilPlayground::LoadAsset<UCurveFloat>("/Script/Engine.CurveFloat'/Game/Data/Curve_ClimbJump.Curve_ClimbJump'");
}


// Called when the game starts
void UClimbComponent::BeginPlay()
{
	Super::BeginPlay();

	ProtagonistCharacter = Cast<AProtagonistCharacter>(GetOwner());
	ensure(ProtagonistCharacter != nullptr);
	CapsuleHalfHeight = ProtagonistCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	CapsuleRadius = ProtagonistCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();

	AnimInstance = Cast<UProtagonistAnimInstance>(ProtagonistCharacter->GetMesh()->GetAnimInstance());

	WalkableAngle = ProtagonistCharacter->GetCharacterMovement()->GetWalkableFloorAngle();

	const APlayerController* PlayerController = Cast<APlayerController>(ProtagonistCharacter->Controller);
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	ensure(Subsystem != nullptr);
	if (Subsystem->HasMappingContext(PreClimbingMappingContext) == false)
	{
		Subsystem->AddMappingContext(PreClimbingMappingContext, ProtagonistCharacter->InputPriority);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(ProtagonistCharacter->GetLocalViewingPlayerController()->InputComponent))
	{
		// none 바인딩 이유 : 키 누르고 있는 상태에서 우선순위 높은 컨텍스트 제거하면, 교체한 컨텍스트의 trigger로는 호출안되기 때문
		// EnhancedInputComponent->BindAction(GrabWallAction, ETriggerEvent::None, this, &UClimbComponent::GrabWall);
		EnhancedInputComponent->BindAction(GrabWallAction, ETriggerEvent::Triggered, this, &UClimbComponent::GrabWall);

		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Triggered, this, &UClimbComponent::Move);
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Completed, this, &UClimbComponent::Stop);

		EnhancedInputComponent->BindAction(ClimbJumpAction, ETriggerEvent::Started, this, &UClimbComponent::Jump);
		EnhancedInputComponent->BindAction(ClimbFallAction, ETriggerEvent::Started, this, &UClimbComponent::ActiveClimbing, false, true);
	}
}

void UClimbComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ProgressJump();
}


FHitResult UClimbComponent::TraceClimbing(FVector StartLoc, float Distance, EDrawDebugTrace::Type DebugTraceType)
{
	const auto TraceDisplacement = ProtagonistCharacter->GetActorForwardVector() * Distance;
	const auto EndLoc = StartLoc + TraceDisplacement;

	FHitResult HitResult;
	const bool IsHitHead = UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLoc, EndLoc, WallTraceChannel,
	                                                             true, {ProtagonistCharacter},
	                                                             DebugTraceType, HitResult, true);

	return HitResult;
}

void UClimbComponent::ActiveClimbing(bool IsActive, bool IsStand)
{
	RemainJumpDistance = 0;
	// Set State
	ProtagonistCharacter->GetCharacterMovement()->SetMovementMode(IsActive ? MOVE_Flying : MOVE_Falling);
	ProtagonistCharacter->GetCharacterMovement()->StopMovementImmediately();
	ProtagonistCharacter->GetCharacterMovement()->bOrientRotationToMovement = !IsActive;
	ProtagonistCharacter->CharacterCurrentInfo->OnClimbing = IsActive;

	ProtagonistCharacter->WeaponComponent->SetWeaponHidden(IsActive);
	CanControlMoving = IsActive;

	if (IsStand)
	{
		const FRotator Rotator(0, ProtagonistCharacter->GetActorRotation().Yaw, 0);
		ProtagonistCharacter->SetActorRotation(Rotator);
	}

	if (IsActive)
	{
		if (Subsystem->HasMappingContext(ClimbMappingContext) == false)
			Subsystem->AddMappingContext(ClimbMappingContext, ProtagonistCharacter->InputPriority + 1);
	}
	else
	{
		if (Subsystem->HasMappingContext(ClimbMappingContext))
			Subsystem->RemoveMappingContext(ClimbMappingContext);
	}
}


void UClimbComponent::GrabWall()
{
	if (ProtagonistCharacter->GetCharacterMovement()->MovementMode != MOVE_Walking)
		return;

	auto HitHeadResult = TraceClimbing(GetHeadLocation());
	if (HitHeadResult.bBlockingHit)
	{
		auto HitHipsResult = TraceClimbing(ProtagonistCharacter->GetActorLocation());
		if (HitHipsResult.bBlockingHit)
		{
			auto WallRot = FRotationMatrix::MakeFromX(HitHipsResult.Normal * -1).Rotator();
			// UE_LOG(LogTemp,Log,TEXT("WallRot : %s"), *WallRot.ToString());
			if (FMath::Abs(WallRot.Pitch) < WalkableAngle)
			{
				ActiveClimbing(true);

				auto ClimbDeltaLoc = HitHipsResult.Location + HitHipsResult.Normal * 50 - ProtagonistCharacter->GetActorLocation();
				auto CapsuleRot = ProtagonistCharacter->GetCapsuleComponent()->GetComponentRotation();
				auto ClimbRot = FRotator(WallRot.Pitch, WallRot.Yaw, CapsuleRot.Roll);

				ProtagonistCharacter->GetCapsuleComponent()->MoveComponent(ClimbDeltaLoc, ClimbRot, false);
			}
		}
	}
}

void UClimbComponent::SetCanControlMoving(bool IsCan)
{
	CanControlMoving = IsCan;
	ProtagonistCharacter->GetCharacterMovement()->StopMovementImmediately();
}


void UClimbComponent::Jump()
{
	// 캐릭터 반대로 보고 있으면 낙하
	const auto ProtagonistForwardVector = ProtagonistCharacter->GetActorForwardVector();
	const auto ControllerForwardVector = FRotationMatrix(ProtagonistCharacter->GetControlRotation()).GetScaledAxis(EAxis::X);
	const double DotValue = ProtagonistForwardVector.Dot(ControllerForwardVector);
	if (DotValue < 0)
	{
		ActiveClimbing(false);
		return;
	}

	if (ProtagonistCharacter->CharacterCurrentInfo->InputDir.X < 0)
	{
		StartJump(FVector2D(-1, 0), FName("JumpLeft"));
	}
	else if (ProtagonistCharacter->CharacterCurrentInfo->InputDir.X > 0)
	{
		StartJump(FVector2D(1, 0), FName("JumpRight"));
	}
	else if (ProtagonistCharacter->CharacterCurrentInfo->InputDir.Y > 0)
	{
		StartJump(FVector2D(0, 1), FName("JumpUp"));
	}
}

void UClimbComponent::StartJump(FVector2D Dir, FName SectionName, float InPlayRate)
{
	JumpDir = Dir;
	SetCanControlMoving(false);
	RemainJumpDistance = JumpDistance;
	LastJumpLoc = ProtagonistCharacter->GetActorLocation();
	PlayMontage(SectionName, InPlayRate);
}

void UClimbComponent::ProgressJump()
{
	if (RemainJumpDistance > 0)
	{
		RemainJumpDistance -= FVector::Distance(LastJumpLoc, ProtagonistCharacter->GetActorLocation());
		LastJumpLoc = ProtagonistCharacter->GetActorLocation();

		const float JumpSpeed = JumpSpeedCurve->GetFloatValue(RemainJumpDistance);
		Climb(JumpDir, JumpSpeed);
		if (RemainJumpDistance <= 0)
		{
			SetCanControlMoving(true);
		}
	}
}


void UClimbComponent::PlayMontage(FName SectionName, float InPlayRate) const
{
	AnimInstance->Montage_Play(ClimbMontage, InPlayRate);
	AnimInstance->Montage_JumpToSection(SectionName);
}

FVector UClimbComponent::GetHeadLocation() const
{
	return ProtagonistCharacter->GetActorLocation() + ProtagonistCharacter->GetActorUpVector() * CapsuleHalfHeight;
}

bool UClimbComponent::CanClimbOver(FVector& ClimbOverLoc)
{
	bool IsExistingStandSpace = false;
	const FVector TraceDir = FVector(ProtagonistCharacter->GetActorForwardVector().X, ProtagonistCharacter->GetActorForwardVector().Y, 0);

	FHitResult HeadHitResult;;
	auto HeadTraceStartLoc = GetHeadLocation();
	auto HeadTraceEndLoc = HeadTraceStartLoc + TraceDir * CapsuleRadius * 4.f;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetHeadLocation(), HeadTraceEndLoc,
	                                      WallTraceChannel, true,
	                                      {ProtagonistCharacter},
	                                      EDrawDebugTrace::None, HeadHitResult, true);

	if (!HeadHitResult.bBlockingHit)
	{
		auto SpaceTraceStartLoc = ProtagonistCharacter->GetActorLocation() + FVector::UpVector * CapsuleHalfHeight * 3;
		auto SpaceTraceEndLoc = SpaceTraceStartLoc + TraceDir * CapsuleRadius * 2.f;

		FHitResult HitSpaceResult;
		UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), SpaceTraceStartLoc, SpaceTraceEndLoc, CapsuleRadius, CapsuleHalfHeight,
		                                         VisibilityChannel, true, {ProtagonistCharacter},
		                                         EDrawDebugTrace::None, HitSpaceResult, true);

		if (!HitSpaceResult.bBlockingHit)
		{
			ClimbOverLoc = SpaceTraceEndLoc;
			IsExistingStandSpace = true;
		}
	}

	return IsExistingStandSpace;
}


void UClimbComponent::Stop()
{
	ProtagonistCharacter->CharacterCurrentInfo->InputDir = {0, 0};
	ProtagonistCharacter->GetCharacterMovement()->StopMovementImmediately();
}


void UClimbComponent::Move(const FInputActionValue& Value)
{
	if (!CanControlMoving)
	{
		ProtagonistCharacter->CharacterCurrentInfo->InputDir = FVector2d::Zero();
		return;
	}

	const auto InputVector = Value.Get<FVector2D>();
	// 방향 전환시 가속도 제거
	if (ProtagonistCharacter->CharacterCurrentInfo->InputDir != InputVector)
	{
		ProtagonistCharacter->GetCharacterMovement()->StopMovementImmediately();
	}
	ProtagonistCharacter->CharacterCurrentInfo->InputDir = InputVector;

	Climb(ProtagonistCharacter->CharacterCurrentInfo->InputDir);
}

void UClimbComponent::Climb(FVector2D InputDir, float Speed)
{
	FVector Offset = ProtagonistCharacter->GetActorRightVector() * InputDir.X + ProtagonistCharacter->GetActorUpVector() * InputDir.Y;

	Offset = Offset.GetSafeNormal() * 30;
	auto HitResult = TraceClimbing(ProtagonistCharacter->GetActorLocation() + Offset);

	if (HitResult.bBlockingHit)
	{
		const auto ProtagonistRotation = ProtagonistCharacter->GetActorRotation();
		const auto ProtagonistLocation = ProtagonistCharacter->GetActorLocation();

		auto WallRot = FRotationMatrix::MakeFromX(HitResult.Normal * -1).Rotator();
		auto ClimbRot = FRotator(WallRot.Pitch, WallRot.Yaw, ProtagonistRotation.Roll);
		ProtagonistCharacter->SetActorRotation(FMath::RInterpTo(ProtagonistRotation, ClimbRot, GetWorld()->DeltaTimeSeconds, 5));

		auto MovingDir = (HitResult.Location - ProtagonistLocation).GetSafeNormal() * Speed * GetWorld()->DeltaTimeSeconds;
		ProtagonistCharacter->AddMovementInput(MovingDir);
	}


	if (!HitResult.bBlockingHit) // 짚을 벽이 없으면 낙하
	{
		ActiveClimbing(false);
	}
	else if (FVector ClimbOverLoc; InputDir.Y > 0 && CanClimbOver(ClimbOverLoc))
	{
		FLatentActionInfo LatentActionInfo;
		LatentActionInfo.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(ProtagonistCharacter->GetCapsuleComponent(), ClimbOverLoc,
		                                      FRotator(0, ProtagonistCharacter->GetActorRotation().Yaw, 0),
		                                      true, true, 0.3f, false, EMoveComponentAction::Move, LatentActionInfo);
		ActiveClimbing(false, false);
		PlayMontage("ClimbOver");
	}
	else if (InputDir.Y < 0 && CanLand())
	{
		ActiveClimbing(false);
	}
}

bool UClimbComponent::CanLand()
{
	const auto TraceStartLoc = ProtagonistCharacter->GetActorLocation() - ProtagonistCharacter->GetActorUpVector() * CapsuleHalfHeight;
	const auto TraceEndLoc = TraceStartLoc - ProtagonistCharacter->GetActorUpVector() * 30;
	FHitResult HitResult;
	const bool IsLand = UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStartLoc, TraceEndLoc,
	                                                          WallTraceChannel,
	                                                          true, {ProtagonistCharacter},
	                                                          EDrawDebugTrace::None, HitResult, true);

	return IsLand;
}
