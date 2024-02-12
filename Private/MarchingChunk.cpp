// Fill out your copyright notice in the Description page of Project Settings.


#include "MarchingChunk.h"
#include "FastNoiseLite.h"

AMarchingChunk::AMarchingChunk()
{
	Voxels.SetNum((Size + 1) * (Size + 1) * (Size + 1));
}

void AMarchingChunk::GenerateHeightMap()
{
	const auto Position = GetActorLocation() / 100;

	for (int x = 0; x <= Size; ++x)
	{
		for (int y = 0; y <= Size; ++y)
		{
			for (int z = 0; z <= Size; ++z)
			{
				Voxels[GetVoxelIndex(x, y, z)] = Noise->GetNoise(x + Position.X, y + Position.Y, z + Position.Z);
			}
		}
	}
}

void AMarchingChunk::GenerateMesh()
{
	if (SurfaceLevel > 0.0f) //triangles face correct direction based on surface level
	{
		TriangleOrder[0] = 0;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 2;
	}
	else
	{
		TriangleOrder[0] = 2;
		TriangleOrder[1] = 1;
		TriangleOrder[2] = 0;
	}

	float Cube[8];

	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			for (int z = 0; z < Size; ++z)
			{
				for (int i = 0; i < 8; ++i)
				{
					Cube[i] = Voxels[GetVoxelIndex(x + VertexOffset[i][0], y + VertexOffset[i][1], z + VertexOffset[i][2])];
				}
				March(x, y, z, Cube);
			}
		}
	}

}

void AMarchingChunk::March(int X, int Y, int Z, const float Cube[8])
{
	int VertexMask = 0;
	FVector EdgeVertex[12];

	for (int i = 0; i < 8; ++i)
	{
		if (Cube[i] <= SurfaceLevel)	//check 8 cube values
			VertexMask |= 1 << i; //create the vertex mask showing where the vertex are
	}

	//look at our preset values and get the one with bits for where our edges are
	const int EdgeMask = CubeEdgeFlags[VertexMask]; 
	
	if (EdgeMask == 0) return; //no edges to be drawn (air block)

	for (int i = 0; i < 12; ++i)
	{
		if ((EdgeMask & 1 << i) != 0) //we need an edge here
		{
			//get the current edge we want and look at the two vertex
			const float Offset = Interpolation ? GetInterpolationOffset(Cube[EdgeConnection[i][0]], Cube[EdgeConnection[i][1]]): 0.5f; 
			
			EdgeVertex[i].X = X + (VertexOffset[EdgeConnection[i][0]][0] + Offset * EdgeDirection[i][0]);
			EdgeVertex[i].Y = Y + (VertexOffset[EdgeConnection[i][0]][1] + Offset * EdgeDirection[i][1]);
			EdgeVertex[i].Z = Z + (VertexOffset[EdgeConnection[i][0]][2] + Offset * EdgeDirection[i][2]);
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		//given the mask (where our vertices are) look for a corresponding triangle 
		// (3*i as triangles have 3 vertices), if its not -1, a triangle should be drawn
		if (TriangleConnectionTable[VertexMask][3 * i] < 0) break; 

		auto V1 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i]] * 100; //get a vertex of the triangle we should draw
		auto V2 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 1]] * 100;
		auto V3 = EdgeVertex[TriangleConnectionTable[VertexMask][3 * i + 2]] * 100;

		auto Normal = FVector::CrossProduct(V2 - V1, V3 - V1); //cross product of two lines gives perpendicular line (normal to plane)
		Normal.Normalize();

		MeshData.Vertices.Add(V1);
		MeshData.Vertices.Add(V2);
		MeshData.Vertices.Add(V3);

		MeshData.Triangles.Add(VertexCount + TriangleOrder[0]);
		MeshData.Triangles.Add(VertexCount + TriangleOrder[1]);
		MeshData.Triangles.Add(VertexCount + TriangleOrder[2]);

		MeshData.Normals.Add(Normal);
		MeshData.Normals.Add(Normal);
		MeshData.Normals.Add(Normal);

		VertexCount += 3;
	}
}

int AMarchingChunk::GetVoxelIndex(int X, int Y, int Z) const
{
	return Z * (Size + 1) * (Size + 1) + Y * (Size + 1) + X;
}

float AMarchingChunk::GetInterpolationOffset(float V1, float V2) const
{
	const float Delta = V2 - V1;
	return Delta == 0.0f ? SurfaceLevel : (SurfaceLevel - V1) / Delta;
}
