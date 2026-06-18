// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/SwordActor.h"

#include "EnhancedInputComponent.h"
#include "LevelSequenceActor.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "LevelSequencePlayer.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Character/Bot/BotCharacter.h"
#include "Character/Bot/DamageType_KnockOut.h"
#include "Character/Protagonist/ProtagonistMovementComponent.h"
#include "Component/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/PackageMapClient.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Utils/UtilPlayground.h"

ASwordActor::ASwordActor()
{
}

void ASwordActor::BeginPlay()
{
	Super::BeginPlay();

	PG_LOG(LogPGNetwork, Warning,TEXT(""));
	if (HasAuthority() == false)
	{
		InitTrail();

		AttackMontageEndEventMap.Add(DefaultAttackMontage, FSimpleDelegate::CreateUObject(this, &ASwordActor::GroundAttackMontageEndEvent));
		AttackMontageEndEventMap.Add(UpperAttackMontage, FSimpleDelegate::CreateUObject(this, &ASwordActor::JumpUpperAttackMontageEndEvent));

		FMovieSceneSequencePlaybackSettings UpperSequenceSettings;
		UpperAttackSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), UpperAttackSequence,
		                                                                            UpperSequenceSettings, SequenceActor);
	}
}

void ASwordActor::Destroyed()
{
	Super::Destroyed();
	PG_LOG(LogPGNetwork, Log, TEXT("%s Destroyed"), *GetName());
}

void ASwordActor::BindInputActionsImpl(UEnhancedInputComponent* EnhancedInputComponent)
{
	Super::BindInputActionsImpl(EnhancedInputComponent);

	EnhancedInputComponent->BindAction(UpperAttackTriggerInputAction, ETriggerEvent::Triggered, this, &ASwordActor::TriggerUpperAttack);
	EnhancedInputComponent->BindAction(LowerAttackTriggerInputAction, ETriggerEvent::Triggered, this, &ASwordActor::LowerAttack);
}

void ASwordActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASwordActor, CurrentAttackSectionID);
	DOREPLIFETIME(ASwordActor, CurrentAttackType);
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

void ASwordActor::InitTrail()
{
	const auto TrailPos = 0.5f * (MeshComponent->GetSocketLocation(TrailSocketTopName) + MeshComponent->
		GetSocketLocation(TrailSocketBotName));
	const FRotator TrailRot = FRotationMatrix::MakeFromZ(
		MeshComponent->GetSocketLocation(TrailSocketTopName) - MeshComponent->GetSocketLocation(TrailSocketBotName)).Rotator();


	TrailComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, MeshComponent, TrailSocketBotName, TrailPos, TrailRot,
	                                                              EAttachLocation::KeepWorldPosition, false);


	if (ensureAlwaysMsgf(TrailComponent != nullptr, TEXT("TrailComponent is Null")))
	{
		TrailComponent->SetRenderCustomDepth(true);
		TrailComponent->SetCustomDepthStencilValue(1 << 1);
		TrailComponent->DeactivateImmediate();
	}
}

void ASwordActor::SetCurrentAttackType(Sword::EAttackType AttackType)
{
	CurrentAttackType = AttackType;
	if (OwnerProtagonist->IsLocallyControlled())
	{
		ServerSetCurrentAttackType(AttackType);
	}
}

void ASwordActor::ServerSetCurrentAttackType_Implementation(Sword::EAttackType AttackType)
{
	CurrentAttackType = AttackType;
}

void ASwordActor::PlayMontage(UAnimMontage* Montage, FName SectionName)
{
	if (ensureAlways(ProtagonistAnimInstance != nullptr))
	{
		ProtagonistAnimInstance->Montage_Play(Montage, 1.f);
		if (SectionName.IsNone() == false)
		{
			ProtagonistAnimInstance->Montage_JumpToSection(SectionName);
		}
	}
}

void ASwordActor::ApplyDamageBot(ABotCharacter* Bot, bool bKnockOut) const
{
	const TSubclassOf<UDamageType> DamageType = bKnockOut ? UDamageType_KnockOut::StaticClass() : UDamageType::StaticClass();
	UGameplayStatics::ApplyDamage(Bot, Damage, GetInstigatorController(),
	                              this->GetOwner(), DamageType);
}


void ASwordActor::AttackOnGround()
{
	if (DefaultAttackMontage != nullptr)
	{
		if (ProtagonistAnimInstance != nullptr)
		{
			const double CurrentSec = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
			const double AttackTimeGap = CurrentSec - LastAttackTime;
			constexpr float AttackDelay = 0.5f;
			if (AttackTimeGap < AttackDelay)
				return;

			LastAttackTime = CurrentSec;

			ServerAttackOnGroundEvent(AttackTimeGap, OwnerProtagonist->GetTransform());

			OwnerProtagonist->SetMovable(false);
			OwnerProtagonist->ZoomOnSlash();
		}
	}
}

TArray<FHitResult> ASwordActor::TraceGroundAttack(const FTransform& TrfProtagonist)
{
	const FVector ForwardVectorProtagonist = TrfProtagonist.GetUnitAxis(EAxis::X);

	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), TrfProtagonist.GetLocation() + ForwardVectorProtagonist * AttackRange.X,
	                                    TrfProtagonist.GetLocation() + ForwardVectorProtagonist * AttackRange.X,
	                                    AttackRange * 0.5f,
	                                    TrfProtagonist.GetRotation().Rotator(),
	                                    AttackTraceType,
	                                    false,
	                                    {OwnerProtagonist},
	                                    EDrawDebugTrace::None,
	                                    HitResults,
	                                    true);

	return HitResults;
}

void ASwordActor::GroundAttackToBots(const FTransform& TrfProtagonist, const TArray<FHitResult>& HitResults)
{
	const FVector ForwardVectorProtagonist = TrfProtagonist.GetUnitAxis(EAxis::X);

	TArray<ABotCharacter*> Bots;
	LastGroundHitBots.Reset();

	const bool bKnockOut = CurrentAttackSectionID == DefaultAttackSectionMaxID;
	for (auto HitResult : HitResults)
	{
		ABotCharacter* Bot = Cast<ABotCharacter>(HitResult.GetActor());
		if (Bot == nullptr)
			continue;

		Bots.Add(Bot);
		LastGroundHitBots.Add(Bot);

		Bot->LaunchCharacter(ForwardVectorProtagonist * 1000, true, true);

		ApplyDamageBot(Bot, bKnockOut);
	}
}

void ASwordActor::ServerAttackOnGroundEvent_Implementation(float AttackTimeGap, FTransform TrfProtagonist)
{
	constexpr float ComboLimitSec = 1.f;
	const bool bFirstAttack = (AttackTimeGap > ComboLimitSec || CurrentAttackSectionID >= DefaultAttackSectionMaxID);
	CurrentAttackSectionID = bFirstAttack ? 1 : CurrentAttackSectionID + 1;


	const TArray<FHitResult> HitResults = TraceGroundAttack(TrfProtagonist);
	GroundAttackToBots(TrfProtagonist, HitResults);

	TArray<FVector_NetQuantize100> HitPoints;
	Algo::Transform(HitResults, HitPoints, [](const FHitResult& Hit) { return Hit.ImpactPoint; });

	MulticastAttackOnGroundEvent(CurrentAttackSectionID, HitPoints);
}

bool ASwordActor::ServerAttackOnGroundEvent_Validate(float AttackTimeGap, FTransform TrfProtagonist)
{
	return AttackTimeGap > 0;
}


void ASwordActor::MulticastAttackOnGroundEvent_Implementation(int32 MontageSectionID, const TArray<FVector_NetQuantize100>& HitPoints)
{
	if (HasAuthority())
		return;

	SetActiveTrail(true);
	PlayMontage(DefaultAttackMontage, GetGroundAttackSectionName(MontageSectionID));

	// todo : hp0 이하 안터지게
	ensureAlways(SlashParticleSystem != nullptr);
	for (const FVector_NetQuantize100 HitPoint : HitPoints)
	{
		const auto Explosion = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
		                                                                      SlashParticleSystem.Get(),
		                                                                      HitPoint);
		Explosion->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}

void ASwordActor::TriggerUpperAttack()
{
	if (CurrentAttackType != Sword::EAttackType::None)
		return;

	SetCurrentAttackType(Sword::EAttackType::Upper);

	CurrentUpperComboNum = 0;
	OwnerProtagonist->JumpCurrentCount = OwnerProtagonist->JumpMaxCount; // 추가 점프 불가

	ServerTriggerUpperAttackEvent();
}


void ASwordActor::ServerTriggerUpperAttackEvent_Implementation()
{
	CurrentUpperComboNum = 0;

	for (const auto Bot : LastGroundHitBots)
	{
		if (!Bot)
			continue;

		Bot->LaunchCharacter(FVector(0, 0, 800), true, true);
	}

	MulticastTriggerUpperAttack();
}


void ASwordActor::MulticastTriggerUpperAttack_Implementation()
{
	if (HasAuthority())
		return;

	PlayMontage(UpperAttackMontage, FName("Trigger"));
	SetActiveTrail(true);
}


void ASwordActor::UpperAttackCombo()
{
	const float MontageProgress = ProtagonistAnimInstance->Montage_GetPosition(UpperAttackMontage);
	const FName CurrentSectionName = ProtagonistAnimInstance->Montage_GetCurrentSection(UpperAttackMontage);

	if (CurrentSectionName.ToString().StartsWith("Attack_") && MontageProgress > 0 && MontageProgress < 0.8f)
		return;

	if (CurrentUpperComboNum >= UpperAttackComboMaxCnt)
		return;

	CurrentUpperComboNum++;

	ServerUpperAttackComboEvent(CurrentUpperComboNum);

	const bool IsLastCombo = (CurrentUpperComboNum == UpperAttackComboMaxCnt);
	if (IsLastCombo)
	{
		UpperAttackSequencePlayer->Play();
	}
}


void ASwordActor::ServerUpperAttackComboEvent_Implementation(int ComboNum)
{
	CurrentUpperComboNum = ComboNum;

	TArray<int32> BotNetGuids;
	for (const auto Bot : LastGroundHitBots)
	{
		if (!Bot)
			continue;
		// note : netguid는 서버에서만 생성됨
		BotNetGuids.Add(Bot->GetNetDriver()->GuidCache->GetNetGUID(Bot).ObjectId);

		ApplyDamageBot(Bot);
	}

	MulticastUpperAttackComboEvent(ComboNum, BotNetGuids);
}

void ASwordActor::MulticastUpperAttackComboEvent_Implementation(int ComboNum, const TArray<int32>& BotNetObjectIDs)
{
	PlayMontage(UpperAttackMontage, FName(FString::Printf(TEXT("Attack_%d"), ComboNum)));
	OwnerProtagonist->FixLocation(true);

	const TSharedPtr<FNetGUIDCache> GuidCache = GetWorld()->GetNetDriver()->GuidCache;
	for (const int32 NetObjectID : BotNetObjectIDs)
	{
		FNetworkGUID NetGUID(NetObjectID);
		const ABotCharacter* Bot = Cast<ABotCharacter>(GuidCache->GetObjectFromNetGUID(NetGUID, true));
		if (ensureAlways(Bot))
		{
			Bot->GetCharacterMovement()->Velocity = FVector::Zero();
			Bot->GetCharacterMovement()->GravityScale = 0;
		}
	}
}

void ASwordActor::JumpUpperAttackMontageEndEvent()
{
	PG_LOG(LogPGNetwork, Log, TEXT("%s"), *GetName());
	const bool bFullCombo = (CurrentUpperComboNum == UpperAttackComboMaxCnt);

	if (HasAuthority())
	{
		OwnerProtagonist->MulticastFixLocation(false);
		for (const auto Bot : LastGroundHitBots)
		{
			if (!Bot)
				continue;

			Bot->GetCharacterMovement()->GravityScale = 1;
			if (bFullCombo)
			{
				ApplyDamageBot(Bot, true);
				Bot->LaunchCharacter(FVector(0, 0, -2000), true, true);
			}
		}

		// if (bFullCombo)
		// {
		// 	MulticastJumpUpperFullAttackEvent_Implementation();
		// }
	}
	else
	{
		if (bFullCombo)
		{
			SetActiveTrail(false);
			LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), UpperLastShotFX,
			                                                                             GetActorLocation(), FRotator(0, 0, 90),
			                                                                             FVector(3, 3, 3));
		}
	}
}

void ASwordActor::MulticastJumpUpperFullAttackEvent_Implementation()
{
	SetActiveTrail(false);
	LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), UpperLastShotFX,
	                                                                             GetActorLocation(), FRotator(0, 0, 90),
	                                                                             FVector(3, 3, 3));
}

void ASwordActor::LowerAttack()
{
	if (CurrentAttackType != Sword::None)
		return;

	OwnerProtagonist->JumpCurrentCount = OwnerProtagonist->JumpMaxCount; // 추가 점프 불가
	SetCurrentAttackType(Sword::Lower);

	ServerLowerAttackEvent(OwnerProtagonist->GetTransform());
}

TArray<FHitResult> ASwordActor::TraceLowerAttack(const FTransform& TrfProtagonist)
{
	TArray<FHitResult> HitResults;

	constexpr float Radius = 300.0f;
	constexpr float Height = 500.0f;
	const FVector UpVectorProtagonist = TrfProtagonist.GetUnitAxis(EAxis::Z);

	UKismetSystemLibrary::CapsuleTraceMulti(GetWorld(), TrfProtagonist.GetLocation() - UpVectorProtagonist * Height * 0.5f,
	                                        OwnerProtagonist->GetActorLocation() - UpVectorProtagonist * Height * 0.5f,
	                                        Radius,
	                                        Height,
	                                        AttackTraceType,
	                                        false,
	                                        {OwnerProtagonist},
	                                        EDrawDebugTrace::None,
	                                        HitResults,
	                                        true);

	return HitResults;
}

void ASwordActor::ServerLowerAttackEvent_Implementation(FTransform TrfProtagonist)
{
	OwnerProtagonist->LaunchCharacter(LowerAttackLaunchVelocity, true, true);

	TArray<FHitResult> HitResults = TraceLowerAttack(TrfProtagonist);

	for (auto HitResult : HitResults)
	{
		ABotCharacter* Bot = Cast<ABotCharacter>(HitResult.GetActor());
		if (Bot != nullptr)
		{
			FVector PushDir = (Bot->GetActorLocation() - TrfProtagonist.GetLocation()).GetSafeNormal();
			if (PushDir == FVector::Zero())
			{
				PushDir = TrfProtagonist.GetUnitAxis(EAxis::X);
			}
			PushDir.Z = 0;

			const FVector LaunchVelocity = PushDir * 2000;
			Bot->LaunchCharacter(LaunchVelocity, true, true);
			ApplyDamageBot(Bot);
			// todo slash
		}
	}

	MulticastLowerAttackEvent();
}

void ASwordActor::MulticastLowerAttackEvent_Implementation()
{
	if (HasAuthority())
		return;

	PlayMontage(LowerAttackMontage);

	// FX
	{
		FHitResult HitResultGround;
		UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), OwnerProtagonist->GetActorLocation(),
		                                                OwnerProtagonist->GetActorLocation() + FVector::DownVector * 10000,
		                                                {GroundObjectType}, false, {OwnerProtagonist}, EDrawDebugTrace::None, HitResultGround, true);
		if (HitResultGround.bBlockingHit)
		{
			// UtilPlayground::PrintLog(FString::Printf(TEXT("HitGround Loc : %s"), *HitResultGround.Location.ToString()));
			LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LowerAttackFX,
			                                                                             HitResultGround.Location, FRotator::ZeroRotator,
			                                                                             FVector(2, 2, 2));
		}
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

	if (ProtagonistAnimInstance->Montage_IsPlaying(Montage))
		return;

	AttackMontageEndEventMap[Montage].ExecuteIfBound();
}

void ASwordActor::GroundAttackMontageEndEvent()
{
	if (ProtagonistAnimInstance->Montage_IsPlaying(DefaultAttackMontage))
		return;

	OwnerProtagonist->SetMovable(true);

	if (CurrentAttackType == Sword::Default)
	{
		SetActiveTrail(false);
	}
}

void ASwordActor::Equip(AProtagonistCharacter* TargetCharacter)
{
	Super::Equip(TargetCharacter);
	if (!SlashParticleSystem.IsValid())
	{
		SlashParticleSystem.LoadSynchronous();
	}

	if (ensureAlwaysMsgf(SequenceActor, TEXT("SequenceActor is nullptr")))
	{
		SequenceActor->SetBindingByTag(TEXT("Protagonist"), {OwnerProtagonist});
	}

	if (ensureAlwaysMsgf(OwnerProtagonist, TEXT("Protagonist null")))
	{
		OwnerProtagonist->MovementModeChangedDelegate.AddUniqueDynamic(this, &ASwordActor::OnChangedProtagonistMovementMode);
		ProtagonistAnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ASwordActor::OnMontageEnd);
	}

}

void ASwordActor::UnEquip()
{
	Super::UnEquip();

	OwnerProtagonist->MovementModeChangedDelegate.RemoveDynamic(this, &ASwordActor::OnChangedProtagonistMovementMode);
	ProtagonistAnimInstance->OnMontageEnded.RemoveDynamic(this, &ASwordActor::OnMontageEnd);
}

void ASwordActor::OnChangedProtagonistMovementMode(ACharacter* Character, EMovementMode PrevMovementMode,
                                                   uint8 PreviousCustomMode)
{
	ensureAlways(Character == OwnerProtagonist);

	// note : 자신의 클라이언트에서는 GetReplicatedMovementMode가 항상 move_none으로 찍힘.
	// 다른 캐릭터가 서버와 클라이언트에서 현재 이동상태를 비교해 동기화가 제대로 되고 있는지 확인할 수 있음  

	// PG_LOG(LogPGNetwork,Log,TEXT("%s : MM - %s, replicatedMM - %s"), *Protagonist->GetName(),
	// 	*StaticEnum<EMovementMode>()->GetNameStringByValue(Protagonist->GetCharacterMovement()->MovementMode),
	// 	*StaticEnum<EMovementMode>()->GetNameStringByValue(Protagonist->GetReplicatedMovementMode()));


	switch (OwnerProtagonist->GetCharacterMovement()->MovementMode)
	{
	case MOVE_Walking:
		if (CurrentAttackType == Sword::Lower)
		{
			SpawnLowerAttackLandingFX();
		}

		CurrentAttackType = Sword::Default;
		SetCurrentAttackType(Sword::Default);
		break;

	case MOVE_Falling:
		CurrentAttackType = Sword::None;
		SetCurrentAttackType(Sword::None);
		break;
	}
}

void ASwordActor::SpawnLowerAttackLandingFX()
{
	const FVector LocationFX = OwnerProtagonist->GetTargetLocation() + FVector::DownVector * OwnerProtagonist->GetDefaultHalfHeight();
	const FRotator RotatorFX = FRotator(90, 0, 0);
	const FVector ScaleFX = FVector(1, 1, 2);
	LowerAttackNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), LowerLandingFX,
	                                                                             LocationFX, RotatorFX, ScaleFX);
}

void ASwordActor::SetActiveTrail(bool bActive)
{
	if (bActive)
	{
		TrailComponent->Activate();
	}
	else
	{
		TrailComponent->DeactivateImmediate();
	}
}

void ASwordActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
