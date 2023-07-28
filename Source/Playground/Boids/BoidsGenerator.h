// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/Public/HAL/Runnable.h"
#include "ComputeShader/Public/BoidsComputeShader.h"
#include "BoidsGenerator.generated.h"


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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	class TSubclassOf<class ABoidEntity> BoidEntity;

	TArray<ABoidEntity*> ListEntity;

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	float MovableRadius = 10000;

	UPROPERTY(EditAnywhere, Category=Boids, meta=(AllowPrivateAccess=true))
	int32 EntityCnt = 64;

	virtual bool ShouldTickIfViewportsOnly() const override;

	const float PeriodUpdateDir = 0.1f;
	float ElapsedTime = 0;

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
	FBoidsComputeShaderDispatchParams ComputeShaderDispatchParams;
private:
	UFUNCTION()
	void ComputeShaderResult(int Result);
};
