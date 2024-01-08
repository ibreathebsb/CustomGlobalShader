#pragma once
#include "RenderGraph.h"
#include "CoreMinimal.h"

struct FSimpleParameter
{
	UTextureRenderTarget2D *RenderTarget;
	FVertexBuffer* VertexBuffer;
	FIndexBuffer* IndexBuffer;
	FVector4f Color;
	FVector4f Color2;
	FMatrix44f VPMatrix;
};

/*
 * 	Common Resource
 */
struct FSimpleVertexData
{
	FVector4f Position;
	FVector2f UV;
};

class FSimpleVertexBuffer : public FVertexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	void InitRHI() override
	{
		TResourceArray<FSimpleVertexData, VERTEXBUFFER_ALIGNMENT> Vertices;
		Vertices.SetNumUninitialized(6);

		Vertices[0].Position = FVector4f(1, 1, 0, 1);
		Vertices[0].UV = FVector2f(1, 1);

		Vertices[1].Position = FVector4f(-1, 1, 0, 1);
		Vertices[1].UV = FVector2f(0, 1);

		Vertices[2].Position = FVector4f(1, -1, 0, 1);
		Vertices[2].UV = FVector2f(1, 0);

		Vertices[3].Position = FVector4f(-1, -1, 0, 1);
		Vertices[3].UV = FVector2f(0, 0);

		// Create vertex buffer. Fill buffer with initial data upon creation
		FRHIResourceCreateInfo CreateInfo(TEXT("FRectangleVertexBuffer"), &Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};

class FSimpleIndexBuffer : public FIndexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	void InitRHI() override
	{
		// Indices 0 - 5 are used for rendering a quad. Indices 6 - 8 are used for triangle optimization.
		const uint16 Indices[] = {0, 1, 2, 2, 1, 3};

		TResourceArray<uint16, INDEXBUFFER_ALIGNMENT> IndexBuffer;
		uint32 NumIndices = UE_ARRAY_COUNT(Indices);
		IndexBuffer.AddUninitialized(NumIndices);
		FMemory::Memcpy(IndexBuffer.GetData(), Indices, NumIndices * sizeof(uint16));

		// Create index buffer. Fill buffer with initial data upon creation
		FRHIResourceCreateInfo CreateInfo(TEXT("FRectangleIndexBuffer"), &IndexBuffer);
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexBuffer.GetResourceDataSize(), BUF_Static, CreateInfo);
	}
};

class FSimpleVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FSimpleVertexData);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSimpleVertexData, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FSimpleVertexData, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}
	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

/*
 *  Vertex Resource Declaration
 */
// vertex 和 index 可以自定义,声明用同一个就好了
extern TGlobalResource<FSimpleVertexDeclaration> GSimpleVertexDeclaration;

void RDGDraw(FRHICommandListImmediate &RHIImmCmdList, FSimpleParameter InParameter);
