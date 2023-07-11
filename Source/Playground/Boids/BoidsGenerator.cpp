// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidsGenerator.h"

#include "BoidEntity.h"

// Sets default values
ABoidsGenerator::ABoidsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ABoidsGenerator::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters ActorSpawnParams;

	for (int i = 0; i < BoidsCnt; i++)
	{
		double dx = FMath::RandRange(-1500, 1500);
		double dy = FMath::RandRange(-1500, 1500);
		double dz = FMath::RandRange(-1500, 1500);
		auto Entity = GetWorld()->SpawnActor<ABoidEntity>(BoidEntity, GetActorLocation() + FVector(dx, dy, dz),
		                                                  FRotator::ZeroRotator, ActorSpawnParams);
		ListEntity.Add(Entity);
		Entity->Init(ListEntity[0], GetActorLocation(), MovableRadius);
		Entity->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Turquoise, true);

	// UpdatingBoidDirThread = MakeUnique<FUpdatingBoidDirThread>(ListEntity);
}

void ABoidsGenerator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABoidsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;

	if (ElapsedTime > PeriodUpdateDir)
	{
		ElapsedTime = 0;
		const int32 TaskNum = 5;//FPlatformMisc::NumberOfWorkerThreadsToSpawn();
		ParallelFor(TaskNum, [&](int32 TaskID)
		{
			const int32 EntityStartID = TaskID * BoidsCnt / TaskNum;
			const int32 EntityLastID = TaskID < TaskNum - 1 ? EntityStartID + BoidsCnt / TaskNum - 1 : BoidsCnt - 1;
			for (int i = EntityStartID; i <= EntityLastID; i++)
			{
				ListEntity[i]->CalculateDir();
			}
		}, EParallelForFlags::None);

	}
}


bool ABoidsGenerator::ShouldTickIfViewportsOnly() const
{
	DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Turquoise, false);
	return Super::ShouldTickIfViewportsOnly();
}


/***********************************/
// thread
/***********************************/
ABoidsGenerator::FUpdatingBoidDirThread::FUpdatingBoidDirThread(TArray<ABoidEntity*> _ListEntity)
{
	ListEntity = _ListEntity;
	RunningThread = TUniquePtr<FRunnableThread>(FRunnableThread::Create(this, TEXT("FUpdatingBoidDir")));
}

ABoidsGenerator::FUpdatingBoidDirThread::~FUpdatingBoidDirThread()
{
	UE_LOG(LogTemp, Log, TEXT("FUpdatingBoidDirThread deconstruct"));
	IsKill = true;
	RunningThread->Kill(); // run 빠져나갈때까지 대기 -> exit 호출 
}

uint32 ABoidsGenerator::FUpdatingBoidDirThread::Run()
{
	while (!IsKill)
	{
		for (auto& Entity : ListEntity)
		{
			Entity->CalculateDir();
		}
		FPlatformProcess::Sleep(0.1f);
	}
	return 1;
}

void ABoidsGenerator::FUpdatingBoidDirThread::Exit()
{
	FRunnable::Exit();
	UE_LOG(LogTemp, Log, TEXT("FUpdatingBoidDirThread EXIT"));
}
