// Fill out your copyright notice in the Description page of Project Settings.


#include "ChunkWorld.h"
#include "ChunkBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AChunkWorld::AChunkWorld()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AChunkWorld::BeginPlay()
{
	Super::BeginPlay();

	for (int x = -DrawDistance; x <= DrawDistance; ++x)
	{
		for (int y = -DrawDistance; y <= DrawDistance; ++y)
		{
			auto transform = FTransform(
				FRotator::ZeroRotator,
				FVector(x * ChunkSize * 100, y * ChunkSize * 100, 0),
				FVector::OneVector
			);
			const auto chunk = GetWorld()->SpawnActorDeferred<AChunkBase>(Chunk, transform, this);
			chunk->Material = Material;
			UGameplayStatics::FinishSpawningActor(chunk, transform); 
		}
	}
	
}


