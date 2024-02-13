// Fill out your copyright notice in the Description page of Project Settings.


#include "GreedyChunk.h"

#include "Enums.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseLite.h"

// Sets default values
AGreedyChunk::AGreedyChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();
	Noise->SetFrequency(0.015f);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Blocks.SetNum(Size.X * Size.Y * Size.Z);

	Mesh->SetCastShadow(false);

	SetRootComponent(Mesh);

}

// Called when the game starts or when spawned
void AGreedyChunk::BeginPlay()
{
	Super::BeginPlay();


	GenerateBlocks();

	GenerateMesh();

	ApplyMesh();
	
	UE_LOG(LogTemp, Warning, TEXT("Vertex Count : %d"), VertexCount); // ~2500 per chunk, not optimal growth algorithm, always go along width
}

void AGreedyChunk::GenerateBlocks()
{
	const auto Location = GetActorLocation();

	for (int x = 0; x < Size.X; ++x)
	{
		for (int y = 0; y < Size.Y; ++y)
		{
			const float XPos = (x * 100 + Location.X) / 100;
			const float YPos = (y * 100 + Location.Y) / 100;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(XPos, YPos) + 1) * Size.Z / 2), 0, Size.Z); //get noise is -1 -> 1 , + 1 = 0 -> 2 * size = 0->2size / 2

			for (int z = 0; z < Height; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
			}

			for (int z = Height; z < Size.Z; ++z)
			{
				Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}

		}

	}
}

void AGreedyChunk::ApplyMesh()
{
	Mesh->CreateMeshSection(0, MeshData.Vertices, MeshData.Triangles, TArray<FVector>(), MeshData.UV0, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
}

void AGreedyChunk::GenerateMesh()
{
	for (int Axis = 0; Axis < 3; Axis++) //take a slice equal to a plane in a perpindicular axis
	{
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		const int MainAxisLimit = Size[Axis]; //0 = X, 1 = Y, 2 = Z
		int Axis1Limit = Size[Axis1];
		int Axis2Limit = Size[Axis2];

		auto DeltaAxis1 = FIntVector::ZeroValue; //growth of greedy mesh (width x Height)
		auto DeltaAxis2 = FIntVector::ZeroValue;

		auto ChunkItr = FIntVector::ZeroValue; //the blocks that we are itterating over
		auto AxisMask = FIntVector::ZeroValue; //which axis we are itterating over

		AxisMask[Axis] = 1; // [1,0,0] for x axis

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit); //flattened 2d array

		// check each slice

		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit; ) //on the main axis go from -1 to the limit, slice before block 0, 1 , 2 and after 2
		{	//this "looks" in the x axis for meshes 
			int N = 0;

			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis1Limit; ++ChunkItr[Axis2])
			{	//from the 2d meshes in x axis look at ones going in the y
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis2Limit; ++ChunkItr[Axis1])
				{	//and z
					const auto CurrentBlock = GetBlock(ChunkItr); //current
					const auto CompareBlock = GetBlock(ChunkItr + AxisMask); //get neighbor along current iteration direction //the other side of the mask we are looking at

					const bool CurrentBlockOpaque = CurrentBlock != EBlock::Air;
					const bool CompareBlockOpaque = CompareBlock != EBlock::Air;

					if (CurrentBlockOpaque == CompareBlockOpaque)
					{
						Mask[N++] = FMask{ EBlock::Null, 0 }; //both blocks are air or stone, we dont need the mesh
					}
					else if (CurrentBlockOpaque)
					{
						Mask[N++] = FMask{ CurrentBlock, 1 };
					}
					else
					{
						Mask[N++] = FMask{ CompareBlock, -1 };
					}
				}
			}

			++ChunkItr[Axis];
			N = 0;

			// Generate Mesh from the mask
			for (int j = 0; j < Axis2Limit; j++) //iterate over slice again
			{
				for (int i = 0; i < Axis1Limit; )
				{
					if (Mask[N].Normal != 0) //current mask pos needs a face
					{
						const auto CurrentMask = Mask[N];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int width;

						for (width = 1; i + width < Axis1Limit && CompareMask(Mask[N + width], CurrentMask); ++width) //loop through until current width is equal to the max width
						{}

						int height;
						bool done = false;
						for (height = 1; j + height < Axis2Limit; ++height)
						{
							for (int k = 0; k < width; ++k)
							{
								if (CompareMask(Mask[N + k + height * Axis1Limit], CurrentMask)) continue;

								done = true;
								break;
							}
							if (done) break;
						}

						DeltaAxis1[Axis1] = width;
						DeltaAxis2[Axis2] = height;

						CreateQuad(CurrentMask, AxisMask,
							ChunkItr,//original vertex
							ChunkItr + DeltaAxis1, //+ width
							ChunkItr + DeltaAxis2, //+ height
							ChunkItr + DeltaAxis1 + DeltaAxis2);

						DeltaAxis1 = FIntVector::ZeroValue;
						DeltaAxis2 = FIntVector::ZeroValue;

						for (int l = 0; l < height; l++)
						{
							for (int k = 0; k < width; ++k) 
							{
								Mask[N + k + l * Axis1Limit] = FMask{ EBlock::Null, 0 };
							}
						}

						i += width;
						N += width;
					}
					else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}

void AGreedyChunk::CreateQuad(FMask Mask, FIntVector AxisMask, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4)
{
	const auto Normal = FVector(AxisMask * Mask.Normal);

	MeshData.Vertices.Add(FVector(V1) * 100); // add the vertices
	MeshData.Vertices.Add(FVector(V2) * 100);
	MeshData.Vertices.Add(FVector(V3) * 100);
	MeshData.Vertices.Add(FVector(V4) * 100);

	MeshData.Triangles.Add(VertexCount); //add the triangles
	MeshData.Triangles.Add(VertexCount + 2 + Mask.Normal);
	MeshData.Triangles.Add(VertexCount + 2 - Mask.Normal);
	MeshData.Triangles.Add(VertexCount + 3);
	MeshData.Triangles.Add(VertexCount + 1 - Mask.Normal);
	MeshData.Triangles.Add(VertexCount + 1 + Mask.Normal);

	MeshData.UV0.Add(FVector2D(0, 0)); //set the uv values in the 0 channel
	MeshData.UV0.Add(FVector2D(0, 1)); //not scaled so we can see them stretch
	MeshData.UV0.Add(FVector2D(1, 0 )); //ideally multiply the 1's by width and height (pass them in)
	MeshData.UV0.Add(FVector2D(1/* *width */, 1 /* *height */));

	MeshData.Normals.Add(Normal); //add the normal for the 4 vertex
	MeshData.Normals.Add(Normal);
	MeshData.Normals.Add(Normal);
	MeshData.Normals.Add(Normal);

	VertexCount += 4;

}

int AGreedyChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size.Z * Size.Y + Y * Size.X + X;
}

EBlock AGreedyChunk::GetBlock(FIntVector Index) const
{
	if (Index.X >= Size.X || Index.Y >= Size.Y || Index.Z >= Size.Z || Index.X < 0 || Index.Y < 0 || Index.Z < 0) //faces not culled between chunks
		return EBlock::Air;
	return Blocks[GetBlockIndex(Index.X, Index.Y, Index.Z)];
}

bool AGreedyChunk::CompareMask(FMask M1, FMask M2) const
{
	return M1.Block == M2.Block && M1.Normal == M2.Normal;
}


