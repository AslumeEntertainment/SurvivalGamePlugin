// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FluidDataComponent.h"

#include "Interactables/Items/FluidContainerItem.h"

UFluidDataComponent::UFluidDataComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TSubclassOf<AItemBase> UFluidDataComponent::GetFluidData(AFluidContainerItem* InFluidContainer)
{
	UE_LOG(LogTemp, Log, TEXT("Trying to scoop with %s"), *InFluidContainer->GetClass()->GetName());
	return (FluidData.Contains(InFluidContainer->GetClass())) ? FluidData[InFluidContainer->GetClass()] : nullptr;
}


