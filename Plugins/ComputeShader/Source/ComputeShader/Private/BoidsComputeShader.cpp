#include "BoidsComputeShader.h"
#include <string>

#include "PixelShaderUtils.h"
#include "RenderGraphUtils.h"
#include "RenderGraphUtils.h"

#include "StaticMeshResources.h"
#include "DynamicMeshBuilder.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"
#include "UnifiedBuffer.h"
#include "CanvasTypes.h"
#include "MaterialShader.h"
#include "ShaderParameterMacros.h"
#include "RHIGPUReadback.h"
#include "Misc/KeyChainUtilities.h"
#include "Serialization/MemoryLayout.h"



// DECLARE_STATS_GROUP(TEXT("BoidComputeShader"), STATGROUP_BoidsComputeShader, STATCAT_Advanced);
// DECLARE_CYCLE_STAT(TEXT("BoidComputeShader Execute"), STAT_BoidsComputeShader_Execute, STATGROUP_BoidsComputeShader);

// This class carries our parameter declarations and acts as the bridge between cpp and HLSL.
class COMPUTESHADER_API FBoidsComputeShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsComputeShader);
	// FParameters를 위한 셰이더 바인딩을 생성합니다.
	SHADER_USE_PARAMETER_STRUCT(FBoidsComputeShader, FGlobalShader);


	class FBoidsComputeShader_Perm_TEST : SHADER_PERMUTATION_INT("TEST", 1);

	using FPermutationDomain = TShaderPermutationDomain<
		FBoidsComputeShader_Perm_TEST
	>;
	//
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		/*
		* Here's where you define one or more of the input parameters for your shader.
		* Some examples:
		*/
		// SHADER_PARAMETER(uint32, MyUint32) // On the shader side: uint32 MyUint32;
		// SHADER_PARAMETER(FVector3f, MyVector) // On the shader side: float3 MyVector;

		// SHADER_PARAMETER_TEXTURE(Texture2D, MyTexture) // On the shader side: Texture2D<float4> MyTexture; (float4 should be whatever you expect each pixel in the texture to be, in this case float4(R,G,B,A) for 4 channels)
		// SHADER_PARAMETER_SAMPLER(SamplerState, MyTextureSampler) // On the shader side: SamplerState MySampler; // CPP side: TStaticSamplerState<ESamplerFilter::SF_Bilinear>::GetRHI();

		// SHADER_PARAMETER_ARRAY(float, MyFloatArray, [3]) // On the shader side: float MyFloatArray[3];

		// SHADER_PARAMETER_UAV(RWTexture2D<FVector4f>, MyTextureUAV) // On the shader side: RWTexture2D<float4> MyTextureUAV;
		// SHADER_PARAMETER_UAV(RWStructuredBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: RWStructuredBuffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_UAV(RWBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: RWBuffer<FMyCustomStruct> MyCustomStructs;

		// SHADER_PARAMETER_SRV(StructuredBuffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: StructuredBuffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_SRV(Buffer<FMyCustomStruct>, MyCustomStructs) // On the shader side: Buffer<FMyCustomStruct> MyCustomStructs;
		// SHADER_PARAMETER_SRV(Texture2D<FVector4f>, MyReadOnlyTexture) // On the shader side: Texture2D<float4> MyReadOnlyTexture;

		// SHADER_PARAMETER_STRUCT_REF(FMyCustomStruct, MyCustomStruct)


		SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<int>, Input)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float3>, OutputVelocity)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FBoidsEntityInfo>, BoidsInfos)


		SHADER_PARAMETER(int, EntityCnt)
		SHADER_PARAMETER(float, NeighborRadius)
		SHADER_PARAMETER(float, WeightCohesion)
		SHADER_PARAMETER(float, WeightAlignment)
		SHADER_PARAMETER(float, WeightSeperation)
		SHADER_PARAMETER(float, WeightRandomMove)
		SHADER_PARAMETER(float, WeightLeaderFollowing)
		SHADER_PARAMETER(float, Speed)
		//
		END_SHADER_PARAMETER_STRUCT()

	//
public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		/*
		* Here you define constants that can be used statically in the shader code.
		* Example:
		*/
		// OutEnvironment.SetDefine(TEXT("MY_CUSTOM_CONST"), TEXT("1"));

		/*
		* These defines are used in the thread count section of our shader
		*/
		// OutEnvironment.SetDefine(TEXT("THREADS_X"), 1);
		// OutEnvironment.SetDefine(TEXT("THREADS_Y"), 1);
		// OutEnvironment.SetDefine(TEXT("THREADS_Z"), 1);

		// This shader must support typed UAV load and we are testing if it is supported at runtime using RHIIsTypedUAVLoadSupported
		// OutEnvironment.CompilerFlags.Add(CFLAG_AllowTypedUAVLoads);
	}

public:
};

IMPLEMENT_SHADER_TYPE(, FBoidsComputeShader, TEXT("/ComputeShaderShaders/BoidsComputeShader.usf"), TEXT("BoidsComputeShader"), SF_Compute)

void FBoidsComputeShaderInterface::DispatchRenderThread(FRHICommandListImmediate& RHICmdList,
                                                        FBoidsComputeShaderDispatchParams Params,
                                                        TFunction<void(TArray<FVector3f> OutputVelocity)> AsyncCallback)
{
	FRDGBuilder GraphBuilder(RHICmdList);

	{
		/// profile
		// SCOPE_CYCLE_COUNTER(STAT_BoidsComputeShader_Execute);
		// DECLARE_GPU_STAT(BoidsComputeShader)
		// RDG_EVENT_SCOPE(GraphBuilder, "BoidsComputeShader");
		// RDG_GPU_STAT_SCOPE(GraphBuilder, BoidsComputeShader);

		typename FBoidsComputeShader::FPermutationDomain PermutationVector;

		// Add any static permutation options here
		// PermutationVector.Set<FBoidsComputeShader::FParameters>();


		TShaderMapRef<FBoidsComputeShader> BoidsComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);

		check(BoidsComputeShader.IsValid())
		bool bIsShaderValid = BoidsComputeShader.IsValid();
		if (bIsShaderValid)
		{
			FBoidsComputeShader::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsComputeShader::FParameters>();

			// example SRV
			const void* RawData = static_cast<void*>(Params.Input);
			constexpr int NumInputs = 2;
			constexpr int InputSize = sizeof(int);
			const FRDGBufferRef InputBuffer = CreateUploadBuffer(GraphBuilder, TEXT("InputBuffer"),
			                                                     InputSize, NumInputs, RawData, InputSize * NumInputs);
			PassParameters->Input = GraphBuilder.CreateSRV(FRDGBufferSRVDesc(InputBuffer, PF_R32_SINT));

			///
			PassParameters->EntityCnt = Params.ArrEntityStates.Num();
			PassParameters->NeighborRadius = Params.NeighborRadius;
			PassParameters->WeightAlignment = Params.WeightAlignment;
			PassParameters->WeightCohesion = Params.WeightCohesion;
			PassParameters->WeightSeperation = Params.WeightSeperation;
			PassParameters->WeightLeaderFollowing = Params.WeightLeaderFollowing;
			PassParameters->WeightRandomMove = Params.WeightRandomMove;
			PassParameters->Speed = Params.Speed;
			///


			const FRDGBufferRef BoidsInfoBuffer = GraphBuilder.CreateBuffer(
				FRDGBufferDesc::CreateStructuredDesc(sizeof(FBoidsEntityInfo), Params.ArrEntityStates.Num()),
				TEXT("BoidsInfoBuffer"));
			const FRDGUploadData<FBoidsEntityInfo> UploadInfos(GraphBuilder, Params.ArrEntityStates.Num());
			for (int i = 0; i < UploadInfos.Num(); i++)
			{
				UploadInfos[i] = Params.ArrEntityStates[i];
			}
			GraphBuilder.QueueBufferUpload<FBoidsEntityInfo>(BoidsInfoBuffer, UploadInfos, ERDGInitialDataFlags::NoCopy);
			PassParameters->BoidsInfos = GraphBuilder.CreateUAV(BoidsInfoBuffer, ERDGUnorderedAccessViewFlags::SkipBarrier);


			const FRDGBufferRef OutputBuffer = GraphBuilder.CreateBuffer(
				FRDGBufferDesc::CreateStructuredDesc(sizeof(FVector3f), Params.ArrEntityStates.Num()),
				TEXT("BoidOutputBuffer"));
			PassParameters->OutputVelocity = GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, PF_R32_SINT));


			///
			SetComputePipelineState(RHICmdList, BoidsComputeShader.GetComputeShader());
			auto GroupCount = FComputeShaderUtils::GetGroupCount(Params.GroupCount, FComputeShaderUtils::kGolden2DGroupSize);
			GraphBuilder.AddPass(
				RDG_EVENT_NAME("ExecuteBoidsComputeShader"),
				PassParameters,
				ERDGPassFlags::AsyncCompute,
				[&PassParameters, BoidsComputeShader, GroupCount](FRHIComputeCommandList& RHICmdList)
				{
					FComputeShaderUtils::Dispatch(RHICmdList, BoidsComputeShader, *PassParameters, GroupCount);
				});
			FRHIGPUBufferReadback* GPUBufferReadback = new FRHIGPUBufferReadback(TEXT("ExecuteBoidsComputeShaderOutput"));
			AddEnqueueCopyPass(GraphBuilder, GPUBufferReadback, OutputBuffer, 0u);

			auto RunnerFunc = [GPUBufferReadback, AsyncCallback, Params](auto&& RunnerFunc) -> void
			{
				if (GPUBufferReadback->IsReady())
				{
					const FVector3f* ReadbackOutput = static_cast<FVector3f*>(GPUBufferReadback->Lock(1));
					TArray<FVector3f> OutputVelocity;

					for (int i = 0; i < Params.ArrEntityStates.Num(); i++)
					{
						OutputVelocity.Add(ReadbackOutput[i]);
					}

					GPUBufferReadback->Unlock();

					AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutputVelocity]()
					{
						AsyncCallback(OutputVelocity);
					});
					delete GPUBufferReadback;
				}
				else
				{
					AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]()
					{
						RunnerFunc(RunnerFunc);
					});
				}
			};

			AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunc]()
			{
				RunnerFunc(RunnerFunc);
			});
		}
		else
		{
#if WITH_EDITOR
			GEngine->AddOnScreenDebugMessage((uint64)42145125184, 6.f, FColor::Red, FString(TEXT("The compute shader has a problem.")));
#endif

			// We exit here as we don't want to crash the game if the shader is not found or has an error.
		}
	}

	GraphBuilder.Execute();
}
