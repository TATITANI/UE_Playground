// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterMacros.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "BoidsComputeShader.generated.h"

// 유니폼 버퍼(Uniform Buffers) 는 파라미터를 RHI 리소스로 그룹화합니다.
// 이 리소스는 그 자체로 셰이더 파라미터로서 바인딩됩니다
// BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT or  BEGIN_UNIFORM_BUFFER_STRUCT(FBoidsInfo,)
// 아래 구문 사용 불가 - 글로벌 쉐이더, 유니폼 버퍼 모두 배열 파라미터 미지원
// SHADER_PARAMETER_STRUCT_ARRAY(FBoidsEntityState, BoidStates, [200])
// END_UNIFORM_BUFFER_STRUCT()

struct FBoidsEntityInfo
{
	float Position[3] = {0, 0, 0};
	float Velocity[3] = {0, 0, 0};
	float RandomVector[3] = {0, 0, 0};
};


class COMPUTESHADER_API FBoidsComputeShaderDispatchParams
{
public:
	FIntVector3 GroupCount = FIntVector3(1, 1, 1);
	int Input[2]; // test

	TArray<FBoidsEntityInfo> ArrEntityStates;	

	int32 EntityCnt = 64;
	float NeighborRadius = 500;
	float WeightAlignment = 1;
	float WeightSeperation = 1;
	float WeightCohesion = 1;
	float WeightRandomMove = 1;
	float WeightLeaderFollowing = 1;
	float Speed = 1;
};


// This is a public interface that we define so outside code can invoke our compute shader.
class COMPUTESHADER_API FBoidsComputeShaderInterface
{
public:
	// Executes this shader on the render thread
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FBoidsComputeShaderDispatchParams Params,
		TFunction<void(TArray<FVector3f> OutputVelocity)> AsyncCallback
	);

	// Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
	static void DispatchGameThread(
		FBoidsComputeShaderDispatchParams Params,
		TFunction<void(TArray<FVector3f> OutputVelocity)> AsyncCallback
	)
	{
		ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
			[&Params, AsyncCallback](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(RHICmdList, Params, AsyncCallback);
			});

		FRenderCommandFence Fence;
		Fence.BeginFence();
		Fence.Wait();
	}


	// Dispatches this shader. Can be called from any thread
	static void Dispatch(FBoidsComputeShaderDispatchParams Params, TFunction<void(TArray<FVector3f> OutputVelocity)> AsyncCallback)
	{
		if (IsInRenderingThread())
		{
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, AsyncCallback);
		}
		else
		{
			DispatchGameThread(Params, AsyncCallback);
		}
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoidsComputeShaderLibrary_AsyncExecutionCompleted, TArray<FVector3f>, Output);

UCLASS() // Change the _API to match your project
class COMPUTESHADER_API UComputeShaderLibrary_AsyncExecution : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	int Arg1, Arg2;
	// Execute the actual load
	virtual void Activate() override
	{
		// Create a dispatch parameters struct and fill it the input array with our args
		FBoidsComputeShaderDispatchParams Params;
		Params.Input[0] = Arg1;
		Params.Input[1] = Arg2;


		// Dispatch the compute shader and wait until it completes
		FBoidsComputeShaderInterface::Dispatch(Params, [this](TArray<FVector3f> OutputVelocity)
		{
			this->Completed.Broadcast(OutputVelocity);
		});
	}


	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "ComputeShader", WorldContext = "WorldContextObject"))
	static UComputeShaderLibrary_AsyncExecution* ExecuteBaseComputeShader(UObject* WorldContextObject, int Arg1, int Arg2)
	{
		UComputeShaderLibrary_AsyncExecution* Action = NewObject<UComputeShaderLibrary_AsyncExecution>();
		Action->Arg1 = Arg1;
		Action->Arg2 = Arg2;
		Action->RegisterWithGameInstance(WorldContextObject);

		return Action;
	}

	UPROPERTY(BlueprintAssignable)
	FOnBoidsComputeShaderLibrary_AsyncExecutionCompleted Completed;
};
