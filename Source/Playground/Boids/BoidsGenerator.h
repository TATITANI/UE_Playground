// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "Core/Public/HAL/Runnable.h"
#include "ComputeShader/Public/BoidsComputeShader.h"
#include "BoidsGenerator.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FBoidsWeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightAlignment = 1;

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightSeperation = 2;

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightCohesion = 1;

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightRandomMove = 1;

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	float WeightLeaderFollowing = 1;
};

UENUM(BlueprintType)
enum class EBoidsThreading : uint8
{
	CPU,
	ComputeShader
};


UCLASS()
class PLAYGROUND_API ABoidsGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABoidsGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	class TSubclassOf<class ABoidEntity> BoidEntity;

	TArray<ABoidEntity*> ListEntity;

	ABoidEntity* LeaderEntity;

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	float MovableRadius = 10000;

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	FBox SpawnBox = FBox(FVector(-200, -200, -200), FVector(200, 200, 200));

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	int32 EntityCnt = 64;

	UPROPERTY(EditAnywhere, Category=Boid, meta=(AllowPrivateAccess=true))
	FBoidsWeight BoidsWeight;

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	float Speed = 100;
	const float PeriodUpdatingDir = 0.1f;
	float ElapsedTime = 0;

private:
	FBoidsComputeShaderDispatchParams ComputeShaderDispatchParams;

	UPROPERTY(EditAnywhere, Category=Thread, meta=(AllowPrivateAccess=true))
	EBoidsThreading ThreadType;

	UPROPERTY(EditAnywhere, Category=Thread,
		meta=( EditCondition = "ThreadType == EBoidsThreading::CPU", EditConditionHides))
	uint8 ThreadCnt = 1;

	void UpdateDirByCpu();
	void UpdateDirByGPU();

private:
	class FUpdatingBoidDirThread : public FRunnable
	{
	public:
		FUpdatingBoidDirThread(TArray<ABoidEntity*> _ListEntity);
		virtual ~FUpdatingBoidDirThread() override;

	private:
		TUniquePtr<FRunnableThread> RunningThread;
		TArray<ABoidEntity*> ListEntity;
		bool IsKill = false;

	private:
		// virtual bool Init() override;
		virtual uint32 Run() override;
		// virtual void Stop() override;
		virtual void Exit() override;
	};

	TUniquePtr<FUpdatingBoidDirThread> UpdatingBoidDirThread;

private: // PSO 
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess, MakeEditWidget), Category= PSO)
	TArray<FVector> ArrFoodLocalLoc;

	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	int8 TargetFoodID = 0;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess), Category= PSO)
	float GBestWeight = 2;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess), Category= PSO)
	float PBestWeight = 2;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess), Category= PSO)
	float Inertia = 0.1f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess), Category= PSO)
	float NeighborRadiusPSO = 500;
	
	TPair<FVector, double> GBestInfo, PBestInfo; // GBest, fitness

	void ResetBestPSO();
	FVector GetVelocityPSO();
	double GetFitnessPSO(FVector EntityLoc, FVector FoodLoc);

private:
	virtual bool ShouldTickIfViewportsOnly() const override;
};
