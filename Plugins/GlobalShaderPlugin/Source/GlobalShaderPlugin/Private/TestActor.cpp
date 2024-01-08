// Fill out your copyright notice in the Description page of Project Settings.

#include "TestActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "TestShader.h"

// Sets default values
ATestActor::ATestActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Component = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeComponent"));
    SetRootComponent(Component);
    // �������õ�����������
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        // ��������������
        Component->SetStaticMesh(CubeMesh.Object);
    }
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterial(TEXT("Material'/GlobalShaderPlugin/NewMaterial.NewMaterial'"));
    if (DefaultMaterial.Succeeded())
    {
        Component->SetMaterial(0, DefaultMaterial.Object);
        Material = DefaultMaterial.Object;
    }

    static ConstructorHelpers::FObjectFinder<UTextureRenderTarget2D> RT(TEXT("TextureRenderTarget2D'/GlobalShaderPlugin/NewTextureRenderTarget2D.NewTextureRenderTarget2D'"));
    if (RT.Succeeded())
    {
        RenderTarget = RT.Object;
    }
    // ����Ĭ�ϰ�ɫ����

    // static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterial(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'"));
    // static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterial(TEXT("Material'/GlobalShaderPlugin/NewMaterial.NewMaterial'"));
    // if (DefaultMaterial.Succeeded())
    //{
    //    Material = UMaterialInstanceDynamic::Create(DefaultMaterial.Object, this, "Mat");
    //    Material->SetTextureParameterValue("Texture", Texture);
    //    Component->SetMaterial(0, Material);
    //}
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ATestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    FSimpleParameter Parameter;
    Parameter.RenderTarget = RenderTarget;
    Parameter.Color = FVector4f(0.0, 1.0, 0.0, 1.0);

    ENQUEUE_RENDER_COMMAND(CaptureCommand)
    (
        [Parameter](FRHICommandListImmediate &RHICmdList)
        {
            RDGDraw(RHICmdList, Parameter);
        });
    UMaterialInstanceDynamic *MID = Component->CreateAndSetMaterialInstanceDynamic(0);
    MID->SetTextureParameterValue("InputTexture", (UTexture *)RenderTarget);
}
