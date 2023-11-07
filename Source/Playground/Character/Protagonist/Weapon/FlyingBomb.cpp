// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Protagonist/Weapon/FlyingBomb.h"

#include "Character/Bot/BotCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFlyingBomb::AFlyingBomb()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
}

// Called when the game starts or when spawned
void AFlyingBomb::BeginPlay()
{
	Super::BeginPlay();
	SphereComponent->OnComponentHit.AddUniqueDynamic(this, &AFlyingBomb::OnHit);
}


void AFlyingBomb::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                        const FHitResult& Hit)
{
	// UE_LOG(LogTemp, Log, TEXT("AFlyingBomb OnHit : %s"), *OtherActor->GetName());

	auto HitLocation = Hit.Location;
	const float Radius = 100;
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	GetWorld()->SweepMultiByChannel(
		OUT HitResults,
		HitLocation - Radius * FVector::One(),
		HitLocation + Radius * Radius * FVector::One(),
		FQuat::Identity,
		ECC_GameTraceChannel3,
		FCollisionShape::MakeSphere(Radius),
		Params);

	bool IsHit = false;
	for (auto& HitResult : HitResults)
	{
		const auto OveralppedActor = HitResult.GetActor();
		if (OveralppedActor->IsA(ABotCharacter::StaticClass()))
		{	
			UE_LOG(LogTemp, Log, TEXT("overlap actor : %s"), *OveralppedActor->GetName());
			IsHit = true;
			UGameplayStatics::ApplyDamage(OveralppedActor, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}

	// DrawDebugSphere(GetWorld(), HitLocation, Radius, 32, IsHit ? FColor::Green : FColor::Orange, false, 0.2f);

	verify(ExplodeParticleSystem != nullptr);
	if (ExplodeParticleSystem != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplodeParticleSystem, HitLocation);
	}

	Destroy();
}


void AFlyingBomb::Fly(FVector Velocity, int32 _Damage)
{
	this->Damage = _Damage;
	ensure(SphereComponent != nullptr);
	SphereComponent->SetPhysicsLinearVelocity(Velocity);
}
