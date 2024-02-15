// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChunkMeshData.h"
#include "ChunkBase.generated.h"

class FastNoiseLite;
class UProceduralMeshComponent;

UCLASS()
class VOXEL_API AChunkBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunkBase();

	UPROPERTY(EditDefaultsOnly, Category = "Chunk")
	int Size = 64;

	UPROPERTY(EditDefaultsOnly, Category = "Chunk")
	float Frequency = 0.01f;

	TObjectPtr<UMaterialInterface> Material;

	TObjectPtr<UMaterialInterface> MaterialWater;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GenerateHeightMap();

	virtual void GenerateMesh();

	TObjectPtr<UProceduralMeshComponent> Mesh;
	FastNoiseLite* Noise;
	FChunkMeshData MeshData;
	FChunkMeshData MeshDataTransparent;
	int VertexCountSolid = 0;
	int VertexCountLiquid = 0;

	void ApplyMesh() const;

private:
	
};
