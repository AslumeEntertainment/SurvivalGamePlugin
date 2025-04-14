// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_ShowDamageIndicator.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Objects/GameplayEffectDataObjects/DamageSourceData.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Misc/DamageIndicator.h"

bool UGC_ShowDamageIndicator::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if(IsValid(MyTarget) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_ShowDamageIndicator::OnExecute_Implementation IsValid(MyTarget) == false"));
		return false;
	}

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyTarget));
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_ShowDamageIndicator::OnExecute_Implementation IsValid(ASC) == false"));
		return false;
	}
	if (ASC->bCanDisplayDamageIndicator == false)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_ShowDamageIndicator::OnExecute_Implementation IsValid(World) == false"));
		return false;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (IsValid(PlayerCharacter) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGC_ShowDamageIndicator::OnExecute_Implementation IsValid(PlayerCharacter) == false"));
		return false;
	}

	if (UKismetMathLibrary::Vector_Distance(PlayerCharacter->GetActorLocation(), FVector(Parameters.Location)) > VisibilityDistance)
	{
		return false;
	}

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(Parameters.GetSourceObject());

	ADamageIndicator* DamageIndicator = World->SpawnActorDeferred<ADamageIndicator>(DamageIndicatorClass, MyTarget->GetActorTransform());

	DamageIndicator->Damage = Parameters.RawMagnitude;
	DamageIndicator->DamageType = IsValid(DamageSourceData) ? DamageSourceData->DamageType : ESG_DamageType::None;
	DamageIndicator->FinishSpawning(MyTarget->GetActorTransform());
	DamageIndicator->SetActorLocation(FVector(Parameters.Location));

	return true;
}
