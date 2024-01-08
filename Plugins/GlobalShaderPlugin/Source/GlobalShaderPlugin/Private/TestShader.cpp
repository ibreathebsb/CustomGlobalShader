#include "TestShader.h"
#include "Engine/TextureRenderTarget2D.h"

#include "PipelineStateCache.h"

#include "GlobalShader.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"
#include "PixelShaderUtils.h"

TGlobalResource<FSimpleVertexDeclaration> GSimpleVertexDeclaration;


class FSimpleRDGVertexShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSimpleRDGVertexShader);
	SHADER_USE_PARAMETER_STRUCT(FSimpleRDGVertexShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	// Add your own VS params here!
	SHADER_PARAMETER(FMatrix44f, VPMatrix)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return true;
	}
};

class FSimpleRDGPixelShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSimpleRDGPixelShader);
	SHADER_USE_PARAMETER_STRUCT(FSimpleRDGPixelShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	// Add your own PS params here!
	SHADER_PARAMETER(FVector4f, Color)
	SHADER_PARAMETER(FVector4f, Color2)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};


BEGIN_SHADER_PARAMETER_STRUCT(FSimpleShaderParameter, )
SHADER_PARAMETER_STRUCT_INCLUDE(FSimpleRDGVertexShader::FParameters, VS)
SHADER_PARAMETER_STRUCT_INCLUDE(FSimpleRDGPixelShader::FParameters, PS)
RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()


IMPLEMENT_GLOBAL_SHADER(FSimpleRDGVertexShader, "/GlobalShaderPlugin/Private/SimplePixelShader.usf", "MainVS", SF_Vertex);
IMPLEMENT_GLOBAL_SHADER(FSimpleRDGPixelShader, "/GlobalShaderPlugin/Private/SimplePixelShader.usf", "MainPS", SF_Pixel);

void RDGDraw(FRHICommandListImmediate &RHIImmCmdList, FSimpleParameter InParameter)
{
	check(IsInRenderingThread());

	// RDG Begin
	FRDGBuilder GraphBuilder(RHIImmCmdList);
	auto RenderTargetRHI = InParameter.RenderTarget->GetResource()->GetTexture2DRHI();
	TRefCountPtr<IPooledRenderTarget> RenderTarget = CreateRenderTarget(RenderTargetRHI, TEXT("Draw Example RenderTarget"));
	FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(RenderTarget);

	FSimpleShaderParameter* Parameters = GraphBuilder.AllocParameters<FSimpleShaderParameter>();
	Parameters->RenderTargets[0] = FRenderTargetBinding(RenderTargetTexture, ERenderTargetLoadAction::ENoAction);
	// vs params
	Parameters->VS.VPMatrix = InParameter.VPMatrix;
	
	// ps params
	Parameters->PS.Color = InParameter.Color;
	Parameters->PS.Color2 = InParameter.Color2;

	const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel;
	FGlobalShaderMap *GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FSimpleRDGVertexShader> VertexShader(GlobalShaderMap);
	TShaderMapRef<FSimpleRDGPixelShader> PixelShader(GlobalShaderMap);

	// data from component
	auto VertexBufferRHI = InParameter.VertexBuffer->VertexBufferRHI;
	auto IndexBufferRHI = InParameter.IndexBuffer->IndexBufferRHI;

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("RDGDraw"),
		Parameters,
		ERDGPassFlags::Raster,
		[VertexBufferRHI, IndexBufferRHI, Parameters, VertexShader, PixelShader, GlobalShaderMap](FRHICommandList &RHICmdList)
		{
			FRHITexture2D *RT = Parameters->RenderTargets[0].GetTexture()->GetRHI()->GetTexture2D();
			RHICmdList.SetViewport(0, 0, 0.0f, RT->GetSizeX(), RT->GetSizeY(), 1.0f);

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;

			// vertex declaration(attribute index)
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GSimpleVertexDeclaration.VertexDeclarationRHI;

			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			RHICmdList.SetStencilRef(0);
			
			// Set parameters
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), Parameters->VS);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters->PS);
			
			// vertex buffer
			RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);

			RHICmdList.DrawIndexedPrimitive(
				IndexBufferRHI, // index buffer
				0, // BaseVertexIndex
				0, // MinIndex
				4, // NumVertices
				0, // StartIndex
				2, // NumPrimitives
				2  // NumInstances
			);
		}
	);

	GraphBuilder.Execute();
}
