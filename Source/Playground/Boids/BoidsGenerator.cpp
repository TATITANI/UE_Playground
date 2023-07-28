// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidsGenerator.h"
#include "BoidEntity.h"


// Sets default values
ABoidsGenerator::ABoidsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	ComputeShaderDispatchParams = FBoidsComputeShaderDispatchParams(FIntVector3(1, 1, 1), EntityCnt);
}

// Called when the game starts or when spawned
void ABoidsGenerator::BeginPlay()
{
	Super::BeginPlay();
	FActorSpawnParameters ActorSpawnParams;
	for (int i = 0; i < EntityCnt; i++)
	{
		double dx = FMath::RandRange(-1500, 1500);
		double dy = FMath::RandRange(-1500, 1500);
		double dz = FMath::RandRange(-1500, 1500);
		auto Entity = GetWorld()->SpawnActor<ABoidEntity>(BoidEntity, GetActorLocation() + FVector(dx, dy, dz),
		                                                  FRotator::ZeroRotator, ActorSpawnParams);
		ListEntity.Add(Entity);
		Entity->Init(ListEntity[0], GetActorLocation(), MovableRadius);
		Entity->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		FBoidsEntityInfo Info;
		ComputeShaderDispatchParams.ArrEntityStates.Add(Info);
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Turquoise, true);
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
		// UE_LOG(LogTemp, Log, TEXT("tick"));

		/// GPU
		for (int32 ID = 0; ID < EntityCnt; ID++)
		{
			FVector EntityLoc = ListEntity[ID]->GetActorLocation();
			FVector EntityVelocity = ListEntity[ID]->GetVelocity();
		
			auto RandomVec = FMath::VRand();
			ListEntity[ID]->SetRandomVec(RandomVec);
			for (int32 j = 0; j < 3; j++)
			{
				ComputeShaderDispatchParams.ArrEntityStates[ID].Position[j] = EntityLoc[j];
				ComputeShaderDispatchParams.ArrEntityStates[ID].Velocity[j] = EntityVelocity[j];
				ComputeShaderDispatchParams.ArrEntityStates[ID].RandomVector[j] = RandomVec[j];
			}
		}
		
		FBoidsComputeShaderInterface::Dispatch(ComputeShaderDispatchParams, [this]()
		{
			for (int32 ID = 0; ID < EntityCnt; ID++)
			{
				const auto Dir = ComputeShaderDispatchParams.ArrEntityStates[ID].Direction;
				ListEntity[ID]->SetDir(FVector(Dir[0], Dir[1], Dir[2]));
				
			}
		});


		/// CPU 
		// constexpr int32 TaskCnt = 5; //FPlatformMisc::NumberOfWorkerThreadsToSpawn();
		// const int32 EntityCntPerTask = EntityCnt/TaskCnt;
		// ParallelFor(TaskCnt, [&](int32 TaskID)
		// {
		// 	const int32 StartEntityID = TaskID * EntityCntPerTask;
		// 	const int32 LastEntityID = TaskID < TaskCnt - 1 ? StartEntityID + EntityCntPerTask - 1 : EntityCnt - 1;
		// 	for (int i = StartEntityID; i <= LastEntityID; i++)
		// 	{
		// 		ListEntity[i]->CalculateDir();
		// 	}
		// }, EParallelForFlags::None);
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

void ABoidsGenerator::ComputeShaderResult(int Result)
{
	UE_LOG(LogTemp, Log, TEXT("ComputeShaderResult : %d"), Result);
}
