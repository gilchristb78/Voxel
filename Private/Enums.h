#pragma once

UENUM(BlueprintType)
enum class EDirection
{
	Forward, Right, Back, Left, Up, Down
};

UENUM(BlueprintType)
enum class EBlock
{
	Null UMETA(DisplayName = "Null"),
	Air UMETA(DisplayName = "Air"),
	Stone UMETA(DisplayName = "Stone"),
	Dirt UMETA(DisplayName = "Dirt"),
	Grass UMETA(DisplayName = "Grass"),
	Water UMETA(DisplayName = "Water")


};