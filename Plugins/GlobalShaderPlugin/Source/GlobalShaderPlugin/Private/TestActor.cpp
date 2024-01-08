// TODO: extract render logic into Component

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
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
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
}

// Called when the game starts or when spawned
void ATestActor::BeginPlay()
{
    Super::BeginPlay();
    InitResource();
}

void ATestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    ReleaseResource();
}

void ATestActor::InitResource()
{
    BeginInitResource(&VertexBuffer);
    BeginInitResource(&IndexBuffer);
}

void ATestActor::ReleaseResource()
{
    BeginReleaseResource(&VertexBuffer);
    BeginReleaseResource(&IndexBuffer);
}


// Called every frame
void ATestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FSimpleParameter Parameter;
    Parameter.RenderTarget = RenderTarget;
    Parameter.Color = FVector4f(0.0, 1.0, 0.0, 1.0);
    Parameter.Color2 = FVector4f(1.0, 0.0, 0.0, 1.0);
    Parameter.IndexBuffer = &IndexBuffer;
    Parameter.VertexBuffer = &VertexBuffer;
    
    // just demo
    FScaleMatrix Scale(FVector(0.5,0.5,1.0));
    Parameter.VPMatrix = FMatrix44f(Scale);

    ENQUEUE_RENDER_COMMAND(CaptureCommand)
    (
        [Parameter](FRHICommandListImmediate &RHICmdList)
        {
			RDGDraw(RHICmdList, Parameter);
		}
    );
    UMaterialInstanceDynamic *MID = Component->CreateAndSetMaterialInstanceDynamic(0);
    MID->SetTextureParameterValue("InputTexture", (UTexture *)RenderTarget);
}


