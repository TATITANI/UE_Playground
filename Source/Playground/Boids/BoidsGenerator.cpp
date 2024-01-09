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
		FVector EntityLocalLoc = FMath::RandPointInBox(SpawnBox);
		auto Entity = GetWorld()->SpawnActor<ABoidEntity>(BoidEntity, GetActorLocation() + EntityLocalLoc,
		                                                  FRotator::ZeroRotator, ActorSpawnParams);

		ListEntity.Add(Entity);
		Entity->Init(ListEntity[0], GetActorLocation(), MovableRadius, Speed, BoidsWeight);
		Entity->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		if (i == 0)
		{
			LeaderEntity = Entity;
		}
	}

	ResetBestPSO();

	{
		ComputeShaderDispatchParams.ArrEntityStates.SetNum(EntityCnt);
		ComputeShaderDispatchParams.WeightAlignment = BoidsWeight.WeightAlignment;
		ComputeShaderDispatchParams.WeightCohesion = BoidsWeight.WeightCohesion;
		ComputeShaderDispatchParams.WeightSeperation = BoidsWeight.WeightSeperation;
		ComputeShaderDispatchParams.WeightLeaderFollowing = BoidsWeight.WeightLeaderFollowing;
		ComputeShaderDispatchParams.WeightRandomMove = BoidsWeight.WeightRandomMove;
		ComputeShaderDispatchParams.Speed = Speed;
	}
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
			UpdateVelocityByCpu();
		}
		else if (ThreadType == EBoidsThreading::ComputeShader)
		{
			UpdateVelocityByGPU();
		}
	}
}

void ABoidsGenerator::UpdateVelocityByCpu()
{
	/// CPU 
	const int32 EntityCntPerTask = EntityCnt / ThreadCnt;
	ParallelFor(ThreadCnt, [&](int32 TaskID)
	{
		const int32 StartEntityID = TaskID * EntityCntPerTask;
		const int32 LastEntityID = TaskID < ThreadCnt - 1 ? StartEntityID + EntityCntPerTask - 1 : EntityCnt - 1;
		for (int i = StartEntityID; i <= LastEntityID; i++)
		{
			if (ListEntity[i] == LeaderEntity)
			{
				ListEntity[i]->SetTargetVelocity(GetVelocityPSO());
			}
			else
			{
				ListEntity[i]->CalculateVelocity();
			}
		}
	}, EParallelForFlags::None);
}

void ABoidsGenerator::UpdateVelocityByGPU()
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
	FBoidsComputeShaderInterface::Dispatch(ComputeShaderDispatchParams, [this](TArray<FVector3f> OutputVelocity)
	{
		for (int32 ID = 0; ID < EntityCnt; ID++)
		{
			const auto Velocity = ListEntity[ID] == LeaderEntity ? GetVelocityPSO() : FVector(OutputVelocity[ID]);
			ListEntity[ID]->SetTargetVelocity(Velocity);
		}
	});
}

void ABoidsGenerator::ResetBestPSO()
{
	GBestInfo = PBestInfo = {
		LeaderEntity->GetActorLocation(), GetFitnessPSO(LeaderEntity->GetActorLocation(), GetActorLocation() + ArrFoodLocalLoc[TargetFoodID])
	};
}

FVector ABoidsGenerator::GetVelocityPSO()
{
	const auto LeaderLoc = LeaderEntity->GetActorLocation();

	// 목적지 도착시 새로운 걸로 변경
	{
		if (FVector::DistSquared(LeaderLoc, GetActorLocation() + ArrFoodLocalLoc[TargetFoodID]) < FMath::Pow(500.f, 2))
		{
			TargetFoodID = (TargetFoodID + FMath::RandRange(1, ArrFoodLocalLoc.Num() - 1)) % ArrFoodLocalLoc.Num();
			ResetBestPSO();
			// UE_LOG(LogTemp, Log, TEXT("Change Food %d"), TargetFoodID);
		}
	}

	// update best
	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByChannel(OverlapResults, LeaderLoc, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic,
	                                  FCollisionShape::MakeSphere(NeighborRadiusPSO));
	// DrawDebugSphere(GetWorld(), LeaderLoc, NeighborRadiusPSO, 16, FColor::Magenta, false, 0.1f);
	for (auto& OverlapResult : OverlapResults)
	{
		const AActor* Neighbor = OverlapResult.GetActor();
		if (Neighbor->IsA(ABoidEntity::StaticClass()))
		{
			const auto EntityLoc = Neighbor->GetActorLocation();
			const double BestFitness = GetFitnessPSO(EntityLoc, GetActorLocation() + ArrFoodLocalLoc[TargetFoodID]);
	
			if (Neighbor == LeaderEntity)
			{
				if (BestFitness < PBestInfo.Value)
				{
					PBestInfo = {EntityLoc, BestFitness};
				}
			}
			else if (BestFitness < GBestInfo.Value)
			{
				GBestInfo = {EntityLoc, BestFitness};
			}
		}
	}

	const FVector LeaderVelocity = Inertia * LeaderEntity->GetVelocity()
		+ GBestWeight * FMath::FRand() * (GBestInfo.Key - LeaderLoc)
		+ PBestWeight * FMath::FRand() * (PBestInfo.Key - LeaderLoc);
	return
		LeaderVelocity;
}


double ABoidsGenerator::GetFitnessPSO(FVector EntityLoc, FVector FoodLoc)
{
	return FVector::DistSquared(EntityLoc, FoodLoc);
}

bool ABoidsGenerator::ShouldTickIfViewportsOnly() const
{
	if (IsSelected())
	{
		// DrawDebugBox(GetWorld(), GetActorLocation(), SpawnBox.GetExtent(), FColor::Green, false);
		// DrawDebugSphere(GetWorld(), GetActorLocation(), MovableRadius, 32, FColor::Orange, false);
	}

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
			Entity->CalculateVelocity();
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
