// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/InteractableInterface.h"

#include "Components/MeshComponent.h"

void IInteractableInterface::SetInteractableHighlight(int StencilValue)
{
	AActor* Self = Cast<AActor>(this);
	if (IsValid(Self) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IInteractableInterface::SetInteractableHighlight IsValid(Self) == false"));
		return;
	}

	UMeshComponent* Mesh = Self->GetComponentByClass<UMeshComponent>();
	if (IsValid(Mesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("IInteractableInterface::SetInteractableHighlight IsValid(Mesh) == false"));
		return;
	}

	Mesh->CustomDepthStencilValue = StencilValue;
	Mesh->bRenderCustomDepth = (StencilValue != 0) ? true : false;
	Mesh->MarkRenderStateDirty();
}
