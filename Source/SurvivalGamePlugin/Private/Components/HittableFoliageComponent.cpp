// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HittableFoliageComponent.h"
#include "Characters/SG_Character.h"


void UHittableFoliageComponent::OnHit(TArray<FGameplayEffectSpecHandle> GameplayEffectSpecHandles)
{
	UE_LOG(LogTemp, Warning, TEXT("HittableFoliage was hit!"));
}

bool UHittableFoliageComponent::bCanBeHit()
{
	return true;
}
