// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/DamageFunctionLibrary.h"

#include "Misc/DamageIndicator.h"

void UDamageFunctionLibrary::ShowDamageIndicator(UWorld* World, float Damage, AActor* HitActor)
{
	if (IsValid(HitActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UDamageFunctionLibrary::ShowDamageIndicator IsValid(HitActor) == false"));
		return;
	}

	ADamageIndicator* DamageIndicator = World->SpawnActorDeferred<ADamageIndicator>(ADamageIndicator::StaticClass(), HitActor->GetActorTransform());
	DamageIndicator->Damage = Damage;
	DamageIndicator->FinishSpawning(HitActor->GetActorTransform());
}
