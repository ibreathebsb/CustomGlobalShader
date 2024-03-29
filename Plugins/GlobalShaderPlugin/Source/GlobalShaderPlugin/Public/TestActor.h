#pragma once

// TODO: extract render logic into Component
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestShader.h"
#include "TestActor.generated.h"


UCLASS()
class ATestActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UStaticMeshComponent *Component;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderDemo)
	UMaterialInterface *Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderDemo)
	class UTextureRenderTarget2D *RenderTarget;

private:
	FSimpleVertexBuffer VertexBuffer;
	FSimpleIndexBuffer IndexBuffer;
	void InitResource();
	void ReleaseResource();
};