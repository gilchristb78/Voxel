// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkBase.h"
#include "FastNoiseLite.h"
#include "ProceduralMeshComponent.h"

// Sets default values
AChunkBase::AChunkBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();

	Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Mesh->SetCastShadow(false);

	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AChunkBase::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateHeightMap();

	GenerateMesh();

	UE_LOG(LogTemp, Warning, TEXT("Vertex Count: %d"), VertexCountSolid + VertexCountLiquid);
	UE_LOG(LogTemp, Warning, TEXT("Liquid Count: %d"), VertexCountLiquid);

	ApplyMesh();
}

void AChunkBase::GenerateHeightMap()
{
}

void AChunkBase::GenerateMesh()
{
}

void AChunkBase::ApplyMesh() const
{
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Normals,
		MeshData.UV0,
		MeshData.Colors,
		TArray<FProcMeshTangent>(), true);

	Mesh->SetMaterial(1, MaterialWater);
	Mesh->CreateMeshSection(1, MeshDataTransparent.Vertices,
		MeshDataTransparent.Triangles,
		MeshDataTransparent.Normals,
		MeshDataTransparent.UV0,
		MeshDataTransparent.Colors,
		TArray<FProcMeshTangent>(), false);
}


