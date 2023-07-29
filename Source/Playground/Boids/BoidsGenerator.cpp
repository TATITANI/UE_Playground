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
	for (int i = 0; i < EntityCnt; i++)
	{
		double dx = FMath::RandRange(-1500, 1500);
		double dy = FMath::RandRange(-1500, 1500);
		double dz = FMath::RandRange(-1500, 1500);
		auto Entity = GetWorld()->SpawnActor<ABoidEntity>(BoidEntity, GetActorLocation() + FVector(dx, dy, dz),
		                                                  FRotator::ZeroRotator, ActorSpawnParams);
		ListEntity.Add(Entity);
		Entity->Init(ListEntity[0], GetActorLocation(), MovableRadius, BoidsWeight);
		Entity->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	ComputeShaderDispatchParams.ArrEntityStates.SetNum(EntityCnt);
	ComputeShaderDispatchParams.WeightAlignment = BoidsWeight.WeightAlignment;
	ComputeShaderDispatchParams.WeightCohesion = BoidsWeight.WeightCohesion;
	ComputeShaderDispatchParams.WeightSeperation = BoidsWeight.WeightSeperation;
	ComputeShaderDispatchParams.WeightLeaderFollowing = BoidsWeight.WeightLeaderFollowing;
	ComputeShaderDispatchParams.WeightRandomMove = BoidsWeight.WeightRandomMove;


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

	if (ElapsedTime > PeriodUpdatingDir)
	{
		ElapsedTime = 0;

		if (ThreadType == EBoidsThreading::CPU)
		{
			UpdateDirByCpu();
		}
		else if (ThreadType == EBoidsThreading::ComputeShader)
		{
			UpdateDirByGPU();
		}
	}
}

void ABoidsGenerator::UpdateDirByCpu()
{
	/// CPU 
	const int32 EntityCntPerTask = EntityCnt / ThreadCnt;
	ParallelFor(ThreadCnt, [&](int32 TaskID)
	{
		const int32 StartEntityID = TaskID * EntityCntPerTask;
		const int32 LastEntityID = TaskID < ThreadCnt - 1 ? StartEntityID + EntityCntPerTask - 1 : EntityCnt - 1;
		for (int i = StartEntityID; i <= LastEntityID; i++)
		{
			ListEntity[i]->CalculateDir();
		}
	}, EParallelForFlags::None);
}

void ABoidsGenerator::UpdateDirByGPU()
{
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
	FBoidsComputeShaderInterface::Dispatch(ComputeShaderDispatchParams, [this](TArray<FVector3f> OutputDir)
	{
		for (int32 ID = 0; ID < EntityCnt; ID++)
		{
			ListEntity[ID]->SetDir(FVector(OutputDir[ID]));
		}
	});
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
	// create 생성시 스레드 실행
	// FRunnableThread::Create -> FRunnable::Init ->(init이 true일 경우) FRunnable::Run -> (Run이 종료될 경우) FRunnable::Exit
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
