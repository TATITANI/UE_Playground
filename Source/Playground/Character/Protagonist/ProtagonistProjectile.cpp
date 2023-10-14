// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProtagonistProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AProtagonistProjectile::AProtagonistProjectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddUniqueDynamic(this, &AProtagonistProjectile::OnHit);
	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AProtagonistProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                   const FHitResult& Hit)
{
	// UE_LOG(LogTemp, Log, TEXT("AProtagonistProjectile hit : %s, %s"), *HitComp->GetName(), *OtherActor->GetName());
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) /* && OtherComp->IsSimulatingPhysics()*/)
	{
		// OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		UGameplayStatics::ApplyDamage(OtherActor, this->Damage, this->GetInstigatorController(),
		                              this, UDamageType::StaticClass());

		ensure(ExplodeParticleSystem != nullptr);
		if (ExplodeParticleSystem != nullptr)
		{
			auto Explosion =  UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplodeParticleSystem, GetActorLocation());
			Explosion->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		}

		Destroy();
	}
}

void AProtagonistProjectile::BeginPlay()
{
	Super::BeginPlay();
}
