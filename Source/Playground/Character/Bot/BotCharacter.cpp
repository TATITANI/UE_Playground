// Fill out your copyright notice in the Description page of Project Settings.


#include "BotCharacter.h"

#include "BotAnimInstance.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABotCharacter::ABotCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UBotAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Bot animinstance cast faield"));
		return;
	}
	AnimInstance->OnAttackHit.AddUObject(this, &ABotCharacter::CheckAttack);
	OnAttackEnd = AnimInstance->OnAttackEnded;
}

// Called every frame
void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABotCharacter::Attack()
{
	AnimInstance->PlayAttackMontage();
}

/**
 * @brief 판정 후 데미지 적용
 */
void ABotCharacter::CheckAttack()
{
	FHitResult hitResult;
	FCollisionQueryParams params(NAME_None, false, this);

	const FVector TraceRelatviePos = GetActorForwardVector() * attackDistance;

	// SweepSingleByChannel 트레이스 채널을 사용해 충돌 감지
	bool IsTrace = GetWorld()->SweepSingleByChannel(
		OUT hitResult,
		GetActorLocation(),
		GetActorLocation() + TraceRelatviePos,
		FQuat::Identity,
		ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(attackRadius),
		params);

	FColor ColorDebugCapsule = IsTrace ? FColor::Orange : FColor::Green;
	const FQuat QuatDebugCapsule = FRotationMatrix::MakeFromZ(TraceRelatviePos).ToQuat();
	DrawDebugCapsule(GetWorld(), GetActorLocation() + TraceRelatviePos * 0.5f,
	                 attackDistance * 0.5f, attackRadius, QuatDebugCapsule, ColorDebugCapsule, false, 2.f);

	AActor* ActorHit = hitResult.GetActor();
	bool ExistsTargetActor = ActorHit != nullptr;
	if (IsTrace && ExistsTargetActor)
	{
		// UE_LOG(LogTemp, Log, TEXT("Hit Actor : %s"), *ActorHit->GetActorLabel());
		UGameplayStatics::ApplyDamage(ActorHit, 5, this->GetController(),
		                              this->GetOwner(), UDamageType::StaticClass());
	}
}
