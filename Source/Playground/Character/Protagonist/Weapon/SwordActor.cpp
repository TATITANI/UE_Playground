// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/SwordActor.h"
#include "EnhancedInputComponent.h"
#include "MyGameInstance.h"
#include "Character/Protagonist/ProtagonistAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void ASwordActor::BeginPlay()
{
	Super::BeginPlay();

	// trail
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

	// stat
	const auto GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ensure(GameInstance != nullptr);
	auto SwordStat = GameInstance->GetWeaponStat<FSwordStat>(GetWeaponType(), FName("1"));
	this->Damage = SwordStat->Damage;
	
}


void ASwordActor::BindInputActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &ASwordActor::Attack);
}

void ASwordActor::Attack()
{
	if (AttackMontage != nullptr)
	{
		if (AnimInstance != nullptr)
		{
			 if(AnimInstance->Montage_IsPlaying(AttackMontage))
			 	return;
			
			AnimInstance->Montage_Play(AttackMontage, 1.f);
			AnimInstance->Montage_JumpToSection(GetSectionName());
			SectionID = (SectionID == SectionMaxID) ? 1 : SectionID + 1;

			Character->SetMovable(false);
			TrailComponent->Activate();

			AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ASwordActor::AttackEndEvent);
			AttackCheck();
		}
	}
}

void ASwordActor::AttackCheck() const
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams params(NAME_None, false, this);

	const FVector TracingRelativePos = Character->GetActorForwardVector() * AttackDistance;
	const FQuat QuatBox = FRotationMatrix::MakeFromZ(TracingRelativePos).ToQuat();

	// 트레이스 채널을 사용해 충돌 감지
	GetWorld()->SweepMultiByChannel(
		OUT HitResults,
		Character->GetActorLocation(),
		Character->GetActorLocation() + TracingRelativePos,
		QuatBox,
		ECC_GameTraceChannel3,
		FCollisionShape::MakeBox(BoxExtent),
		params);

	AActor* ActorHit = nullptr;
	bool IsTrace = false;
	for (auto HitResult : HitResults)
	{
		if (HitResult.GetActor() != Character)
		{
			ActorHit = HitResult.GetActor();
			IsTrace = true;
		}
	}

	FColor ColorDebugBox = IsTrace ? FColor::Orange : FColor::Green;
	DrawDebugBox(GetWorld(), Character->GetActorLocation() + TracingRelativePos * 0.5f,
	             BoxExtent, QuatBox, ColorDebugBox, false, 2.f);

	bool ExistsTargetActor = ActorHit != nullptr;
	if (IsTrace && ExistsTargetActor)
	{
		UGameplayStatics::ApplyDamage(ActorHit, Damage, GetInstigatorController(),
		                              this->GetOwner(), UDamageType::StaticClass());
	}
}

FName ASwordActor::GetSectionName() const
{
	return FName(*FString::Printf(TEXT("Attack%d"), SectionID));
}


void ASwordActor::AttackEndEvent(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage)
	{
		Character->SetMovable(true);
		TrailComponent->DeactivateImmediate();

		AnimInstance->OnMontageEnded.RemoveDynamic(this, &ASwordActor::AttackEndEvent);
	}
}

void ASwordActor::Use(AProtagonistCharacter* TargetCharacter)
{
	Super::Use(TargetCharacter);

}
