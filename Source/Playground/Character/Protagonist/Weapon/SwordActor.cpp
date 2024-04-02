// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/SwordActor.h"
#include "EnhancedInputComponent.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequencePlayer.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/Bot/BotCharacter.h"
#include "Character/Bot/DamageType_KnockOut.h"
#include "Component/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Utils/UtilPlayground.h"

void ASwordActor::BeginPlay()
{
	Super::BeginPlay();

	//FX
	{
		const auto TrailPos = 0.5f * (MeshComponent->GetSocketLocation(TrailSocketTopName) + MeshComponent->GetSocketLocation(TrailSocketBotName));
		const FRotator TrailRot = FRotationMatrix::MakeFromZ(
			MeshComponent->GetSocketLocation(TrailSocketTopName) - MeshComponent->GetSocketLocation(TrailSocketBotName)).Rotator();

		ensure(TrailSystem != nullptr);
		TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, MeshComponent, TrailSocketBotName, TrailPos, TrailRot,
		                                                              EAttachLocation::KeepWorldPosition, false);

		ensure(TrailComponent != nullptr);
		TrailComponent->SetRenderCustomDepth(true);
		TrailComponent->SetCustomDepthStencilValue(1 << 1);
		TrailComponent->Deactivate();
	}

	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	EnhancedInputComponent->BindAction(UpperAttackTriggerInputAction, ETriggerEvent::Triggered, this, &ASwordActor::TriggerUpperAttack);
	EnhancedInputComponent->BindAction(LowerAttackTriggerInputAction, ETriggerEvent::Triggered, this, &ASwordActor::LowerAttack);

	AttackMontageEndEventMap.Add(DefaultAttackMontage, FSimpleDelegate::CreateUObject(this, &ASwordActor::GroundAttackMontageEndEvent));
	AttackMontageEndEventMap.Add(UpperAttackMontage, FSimpleDelegate::CreateUObject(this, &ASwordActor::JumpUpperAttackMontageEndEvent));

	ALevelSequenceActor* SequenceActor;
	FMovieSceneSequencePlaybackSettings UpperSequenceSettings;
	UpperAttackSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), UpperAttackSequence,
	                                                                            UpperSequenceSettings, SequenceActor);
}

void ASwordActor::AttackInputStarted()
{
	switch (CurrentAttackType)
	{
	case Sword::Default:
		AttackOnGround();
		break;

	case Sword::Upper:
		UpperAttackCombo();
		break;
	}


}

void ASwordActor::AttackOnGround()
{
	if (DefaultAttackMontage != nullptr)
	{
		if (AnimInstance != nullptr)
		{
			const double CurrentSec = GetWorld()->GetTimeSeconds();
			const double AttackTimeGap = CurrentSec - LastAttackTime;
			constexpr float AttackDelay = 0.5f;
			constexpr float ComboLimitSec = 1.f;
			if (AttackTimeGap < AttackDelay)
				return;
			LastAttackTime = CurrentSec;

			const bool bFirstAttack = (AttackTimeGap > ComboLimitSec || CurrentAttackSectionID >= DefaultAttackSectionMaxID);
			CurrentAttackSectionID = bFirstAttack ? 1 : CurrentAttackSectionID + 1;

			AnimInstance->Montage_Play(DefaultAttackMontage, 1.f);
			AnimInstance->Montage_JumpToSection(GetGroundAttackSectionName(CurrentAttackSectionID));

			Protagonist->SetMovable(false);
			Protagonist->ZoomOnSlash();

			GroundAttackToBots();
			
			TrailComponent->Activate();
		}
	}
}

void ASwordActor::GroundAttackToBots()
{
	TArray<ABotCharacter*> Bots;

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), Protagonist->GetActorLocation() + Protagonist->GetActorForwardVector() * AttackRange.X,
	                                    Protagonist->GetActorLocation() + Protagonist->GetActorForwardVector() * AttackRange.X,
	                                    AttackRange * 0.5f,
	                                    Protagonist->GetActorRotation(),
	                                    AttackTraceType,
	                                    false,
	                                    {Protagonist},
	                                    EDrawDebugTrace::None,
	                                    HitResults,
	                                    true);

	LastGroundHitBots.Reset();

	const bool bKnockOut = CurrentAttackSectionID == DefaultAttackSectionMaxID;
	for (auto HitResult : HitResults)
	{
		ABotCharacter* Bot = Cast<ABotCharacter>(HitResult.GetActor());
		if (Bot == nullptr)
			continue;
		
		Bots.Add(Bot);
		LastGroundHitBots.Add(Bot);
		Bot->LaunchCharacter(Protagonist->GetActorForwardVector() * 1000, true, true);
		AttackToBot(Bot, HitResult.Location, bKnockOut);
	}
}


void ASwordActor::TriggerUpperAttack()
{
	if (CurrentAttackType != Sword::EAttackType::None)
		return;

	CurrentUpperComboNum = 0;

	CurrentAttackType = Sword::EAttackType::Upper;
	AnimInstance->Montage_Play(UpperAttackMontage);
	AnimInstance->Montage_JumpToSection(FName("Trigger"));

	Protagonist->JumpCurrentCount = Protagonist->JumpMaxCount; // 추가 점프 불가
	// 외부압력을 받아도 밀리지 않음
	Protagonist->GetCapsuleComponent()->BodyInstance.bLockXTranslation = true;
	Protagonist->GetCapsuleComponent()->BodyInstance.bLockYTranslation = true;
	Protagonist->GetCapsuleComponent()->BodyInstance.bLockZTranslation = true;

	TrailComponent->Activate();

	for (const auto Bot : LastGroundHitBots)
	{
		if (!Bot)
			continue;

		Bot->LaunchCharacter(FVector(0, 0, 800), true, true);
	}
}


void ASwordActor::UpperAttackCombo()
{
	const float MontageProgress = AnimInstance->Montage_GetPosition(UpperAttackMontage);
	const FName CurrentSectionName = AnimInstance->Montage_GetCurrentSection(UpperAttackMontage);

	if (CurrentSectionName.ToString().StartsWith("Attack_") && MontageProgress > 0 && MontageProgress < 0.8f)
		return;

	if (CurrentUpperComboNum >= UpperAttackComboMaxCnt)
		return;

	CurrentUpperComboNum++;
	const bool IsLastCombo = (CurrentUpperComboNum == UpperAttackComboMaxCnt);

	AnimInstance->Montage_Play(UpperAttackMontage);
	AnimInstance->Montage_JumpToSection(FName(FString::Printf(TEXT("Attack_%d"), CurrentUpperComboNum)));
	Protagonist->FixLocation(true);

	for (const auto Bot : LastGroundHitBots)
	{
		if (!Bot)
			continue;
		Bot->GetCharacterMovement()->Velocity = FVector::Zero();
		Bot->GetCharacterMovement()->GravityScale = 0;

		if (IsLastCombo == false)
		{
			AttackToBot(Bot, TOptional<FVector>());
		}
	}
	
	if (IsLastCombo)
	{
		UpperAttackSequencePlayer->Play();
	}


}

void ASwordActor::LowerAttack()
{
	if (CurrentAttackType != Sword::None)
		return;

	AnimInstance->Montage_Play(LowerAttackMontage);
	Protagonist->JumpCurrentCount = Protagonist->JumpMaxCount; // 추가 점프 불가
	CurrentAttackType = Sword::Lower;
	Protagonist->LaunchCharacter(FVector(0, 0, -2000), true, true);

	constexpr float Radius = 300.0f;
	constexpr float Height = 500.0f;
	
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), Protagonist->GetActorLocation() - Protagonist->GetActorUpVector() * Height * 0.5f,
	                                        Protagonist->GetActorLocation() - Protagonist->GetActorUpVector() * Height * 0.5f,
	                                        Radius,
	                                        Height,
	                                        AttackTraceType,
	                                        false,
	                                        {Protagonist},
	                                        EDrawDebugTrace::None,
	                                        HitResults,
	                                        true);

	for (auto HitResult : HitResults)
	{
		ABotCharacter* Bot = Cast<ABotCharacter>(HitResult.GetActor());
		if (Bot != nullptr)
		{
			FVector Dir = (Bot->GetActorLocation() - Protagonist->GetActorLocation()).GetSafeNormal();
			if (Dir == FVector::Zero())
				Dir = Protagonist->GetActorForwardVector();
			Dir.Z = 0;

			const FVector LaunchVelocity = Dir * 2000;
			Bot->LaunchCharacter(LaunchVelocity, true, true);
			AttackToBot(Bot, TOptional<FVector>());
		}
	}

	// FX
	{
		FHitResult HitResultGround;
		UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Protagonist->GetActorLocation(),
														Protagonist->GetActorLocation() + FVector::DownVector * 10000,
														{GroundObjectType}, false, {Protagonist}, EDrawDebugTrace::None, HitResultGround, true);
		if (HitResultGround.bBlockingHit)
		{
			LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LowerAttackFX,
																						 HitResultGround.Location, FRotator::ZeroRotator,
																						 FVector(2, 2, 2));
			// UtilPlayground::PrintLog(FString::Printf(TEXT("HitGround Loc : %s"), *HitResultGround.Location.ToString()));
		}
	}
}

void ASwordActor::AttackToBot(ABotCharacter* Bot, TOptional<FVector> HitPoint, bool bKnockOut)
{
	if (Bot->HealthComponent->GetCurrentHP() <= 0)
		return;

	const TSubclassOf<UDamageType> DamageType = bKnockOut ? UDamageType_KnockOut::StaticClass() : UDamageType::StaticClass();
	UGameplayStatics::ApplyDamage(Bot, Damage, GetInstigatorController(),
	                              this->GetOwner(), DamageType);


	ensure(SlashParticleSystem != nullptr);
	if (SlashParticleSystem != nullptr)
	{
		const FVector ParticleLoc = HitPoint.IsSet() ? HitPoint.GetValue() : Bot->GetActorLocation();

		const auto Explosion = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
		                                                                      SlashParticleSystem.Get(),
		                                                                      ParticleLoc);
		Explosion->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}


FName ASwordActor::GetGroundAttackSectionName(int32 SectionID) const
{
	return FName(*FString::Printf(TEXT("Attack%d"), SectionID));
}


void ASwordActor::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (AttackMontageEndEventMap.Contains(Montage) == false)
		return;

	if (AnimInstance->Montage_IsPlaying(Montage))
		return;
	
	AttackMontageEndEventMap[Montage].ExecuteIfBound();
}

void ASwordActor::GroundAttackMontageEndEvent()
{
	if (AnimInstance->Montage_IsPlaying(DefaultAttackMontage))
		return;

	Protagonist->SetMovable(true);

	if(CurrentAttackType == Sword::Default)	
		TrailComponent->DeactivateImmediate();

}

void ASwordActor::JumpUpperAttackMontageEndEvent()
{
	Protagonist->FixLocation(false);

	Protagonist->GetCapsuleComponent()->BodyInstance.bLockXTranslation = false;
	Protagonist->GetCapsuleComponent()->BodyInstance.bLockYTranslation = false;
	Protagonist->GetCapsuleComponent()->BodyInstance.bLockZTranslation = false;

	const bool IsLastCombo = (CurrentUpperComboNum == UpperAttackComboMaxCnt);
	for (const auto Bot : LastGroundHitBots)
	{
		if (!Bot)
			continue;

		Bot->GetCharacterMovement()->GravityScale = 1;
		if (IsLastCombo)
		{
			AttackToBot(Bot, TOptional<FVector>(), true);
			Bot->LaunchCharacter(FVector(0, 0, -2000), true, true);
		}
	}

	if (IsLastCombo)
	{
		LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), UpperLastShotFX,
		                                                                             GetActorLocation(), FRotator(0, 0, 90),
		                                                                             FVector(3, 3, 3));

		TrailComponent->DeactivateImmediate();
	}
}

void ASwordActor::Equip(AProtagonistCharacter* TargetCharacter)
{
	Super::Equip(TargetCharacter);
	if (!SlashParticleSystem.IsValid())
	{
		SlashParticleSystem.LoadSynchronous();
	}

	Protagonist->MovementModeChangedDelegate.AddUniqueDynamic(this, &ASwordActor::OnChangedProtagonistMovementMode);
	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ASwordActor::OnMontageEnd);
}

void ASwordActor::UnEquip()
{
	Super::UnEquip();

	Protagonist->MovementModeChangedDelegate.RemoveDynamic(this, &ASwordActor::OnChangedProtagonistMovementMode);
	AnimInstance->OnMontageEnded.RemoveDynamic(this, &ASwordActor::OnMontageEnd);
}

void ASwordActor::OnChangedProtagonistMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
                                                   uint8 PreviousCustomMode)
{
	ensure(Character == Protagonist);
	switch (EMovementMode CurrentMovementMode = Protagonist->GetCharacterMovement()->MovementMode)
	{
	case MOVE_Walking:
		if (CurrentAttackType == Sword::Lower)
		{
			SpawnLowerAttackLandingFX();
		}

		Protagonist->FixLocation(false);
		CurrentAttackType = Sword::Default;
		break;

	case MOVE_Falling:
		CurrentAttackType = Sword::None;
		break;
	}

	
}

void ASwordActor::SpawnLowerAttackLandingFX()
{
	const FVector LocationFX = Protagonist->GetTargetLocation() + FVector::DownVector * Protagonist->GetDefaultHalfHeight();
	const FRotator RotatorFX = FRotator(90, 0, 0);
	const FVector ScaleFX = FVector(1, 1, 2);
	LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LowerLandingFX,
	                                                                             LocationFX, RotatorFX, ScaleFX);
}
