// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FluidDataComponent.generated.h"

class AItemBase;
class AFluidContainerItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAMEPLUGIN_API UFluidDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UFluidDataComponent();

protected:
	
	UPROPERTY(EditAnywhere, Category = "FluidData")
	TMap<TSubclassOf<AFluidContainerItem>, TSubclassOf<AItemBase>> FluidData;

public:	

	UFUNCTION(BlueprintCallable)
	TSubclassOf<AItemBase> GetFluidData(AFluidContainerItem* InFluidContainer);
		
};
