// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootTableComponent.generated.h"

USTRUCT(BlueprintType)
struct FLootTableEntryDropData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MinAmount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	uint8 MaxAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float DropRate = 100.f;

};

USTRUCT(BlueprintType)
struct FLootTableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AActor>, FLootTableEntryDropData> LootTableData;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAMEPLUGIN_API ULootTableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	ULootTableComponent();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "DropData")
	FDataTableRowHandle StandartDestructLootTable;

	UPROPERTY(EditDefaultsOnly, Category = "DropData")
	FDataTableRowHandle SafeDestructLootTable;

	UPROPERTY(EditDefaultsOnly, Category = "DropData")
	FDataTableRowHandle UnsafeDestructLootTable;

	UPROPERTY(EditDefaultsOnly, Category = "Distribution", meta = (ToolTip = "Should dropped actors be arranged vertically or spread out normally"))
	bool bShouldStackDropsVertically = false;

	UPROPERTY(EditAnywhere, Category = "Distribution", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "The lower percent of the owning actor's height at which dropped actors will spawn"))
	float LowerHeightPercent = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Distribution", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "The upper percent of the owning actor's height at which dropped actors will spawn"))
	float UpperHeightPercent = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Distribution", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "How far can the dropped actors spawn from the center of the owning actor"))
	float Spread = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Distribution", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"),
		meta = (ToolTip = "Defines the limit to how much a dropped actor can be rotated when spawned"))
	float RotationLimit = 0.5f;
		
public:

	void DropLoot(bool bWasDestroyedSafely = false, bool bWasDestroyedUnsafely = false);

};
