// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayEffectCalculations/GEEC_CalculateDamage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "GAS/GameplayEffectCalculations/GEEC_Statics.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "Interactables/Items/DamageDealingItem.h"
#include "Components/DamageDealingComponent.h"
#include "Headers/PhysicalPropertyEnums.h"
#include "Characters/InGameCharacter.h"
#include "Objects/DamageSourceData.h"

DEFINE_LOG_CATEGORY(DamageLog);

UGEEC_CalculateDamage::UGEEC_CalculateDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(DamageStatics().DexterityDef);

	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceSlashingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistancePiercingDef);
	RelevantAttributesToCapture.Add(DamageStatics().ResistanceBludgeoningDef);
}

void UGEEC_CalculateDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//USG_AbilitySystemComponent* InstigatorASC = Cast<USG_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor));

	/*if (IsValid(InstigatorAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(InstigatorAbilitySystemComponent) == false"));
		return;
	}*/
	/*if (IsValid(TargetAbilitySystemComponent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(TargetAbilitySystemComponent) == false"));
		return;
	}*/
	/*
	AActor* InstigatorActor = ExecutionParams.GetOwningSpec().GetEffectContext().GetInstigator();
	if (IsValid(InstigatorActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(InstigatorActor) == false"));
		return;
	}
	*/

	AActor* InstigatorActor = ExecutionParams.GetOwningSpec().GetEffectContext().GetInstigator();

	USG_AbilitySystemComponent* InstigatorASC = IsValid(InstigatorActor) ? Cast<USG_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor)) : nullptr;
	USG_AbilitySystemComponent* TargetASC = Cast<USG_AbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());

	AActor* TargetActor = IsValid(TargetASC) ? TargetASC->GetOwner() : nullptr;

	const UDamageSourceData* DamageSourceData = Cast<UDamageSourceData>(ExecutionParams.GetOwningSpec().GetContext().GetSourceObject());
	if (IsValid(DamageSourceData) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(DamageSourceData) == false"));
		return;
	}

	AActor* EffectCauser = ExecutionParams.GetOwningSpec().GetEffectContext().GetEffectCauser();
	if (IsValid(EffectCauser) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::Execute_Implementation IsValid(EffectCauser) == false"));
		return;
	}

	//-------------DAMAGE-FORMULA-(IN-DEVELOPMENT)-------------//

	FVector HitLocation = ExecutionParams.GetOwningSpec().GetContext().GetOrigin();

	float StrengthMultiplier = GetStrengthMultiplier(InstigatorASC, EffectCauser);
	float InstigatorVelocity = GetInstigatorVelocity_MetersPerSec(InstigatorActor);
	float PenetrationMultiplier = GetPenetrationMultiplier(DamageSourceData, InstigatorVelocity, TargetASC, HitLocation);

	float FinalDamage = FMath::Floor(DamageSourceData->BaseDamage * StrengthMultiplier * PenetrationMultiplier);
	
	if (bShouldPrintDebug)
	{
		UE_LOG(DamageLog, Log, TEXT("%s deals damage to %s using %s"), *InstigatorActor->GetName(), *TargetActor->GetName(), *EffectCauser->GetName());
		UE_LOG(DamageLog, Log, TEXT("Damage Type - %d (Slashing, Piercing, Bludgeoning, Fire)"), (uint8)DamageSourceData->DamageType);
		UE_LOG(DamageLog, Log, TEXT("StrengthMultiplier - %.2f"), StrengthMultiplier);
		UE_LOG(DamageLog, Log, TEXT("InstigatorVelocity - %.2f m/s"), InstigatorVelocity);
		UE_LOG(DamageLog, Log, TEXT("PenetrationMultiplier - %.2f"), PenetrationMultiplier);
		UE_LOG(DamageLog, Log, TEXT("FinalDamage - %.f"), FinalDamage);
	}

	//---------------------------------------------------------//

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().HealthProperty, EGameplayModOp::Additive, -FinalDamage));

	FGameplayCueParameters CueParams = FGameplayCueParameters();
	CueParams.SourceObject = DamageSourceData;
	CueParams.RawMagnitude = FinalDamage;
	CueParams.Location = HitLocation;

	TargetASC->ExecuteGameplayCue(DamageIndicatorGameplayCueTag, CueParams);

	int RandInt = FMath::RandRange(1, 100);
	if (IsValid(InstigatorASC))
	{
		RandInt += InstigatorASC->GetNumericAttribute(USG_CharacterAttributeSet::GetDexterityAttribute());
	}
	
	for (TPair<TSubclassOf<UGameplayEffect>, float> AdditionalEffect : DamageSourceData->AdditionalEffects)
	{
		if (RandInt <= AdditionalEffect.Value)
		{
			TargetASC->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(AdditionalEffect.Key->GetDefaultObject()), DamageSourceData->EffectLevel, ExecutionParams.GetOwningSpec().GetEffectContext());
		}
	}

	FGameplayEventData EventData;
	EventData.Target = TargetActor;
	EventData.Instigator = InstigatorActor;

	if (IsValid(InstigatorASC))
	{
		InstigatorASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Triggered"), &EventData);
	}
	TargetASC->HandleGameplayEvent(FGameplayTag::RequestGameplayTag("CommonEvents.Hit.Received"), &EventData);

}

float UGEEC_CalculateDamage::GetStrengthMultiplier(USG_AbilitySystemComponent* InstigatorASC, AActor* EffectCauser) const
{
	float StrengthMultiplier = 1.f;

	if (IsValid(InstigatorASC) == false)
	{
		return StrengthMultiplier;
	}
	if (IsValid(EffectCauser) == false)
	{
		return StrengthMultiplier;
	}

	EWeight EffectCauserWeight = EWeight::Normal;
	if (IsValid(Cast<ADamageDealingItem>(EffectCauser)))
	{
		EffectCauserWeight = Cast<ADamageDealingItem>(EffectCauser)->ItemData.ItemWeight;
	}

	StrengthMultiplier = InstigatorASC->GetNumericAttribute(USG_CharacterAttributeSet::GetStrengthAttribute()) / (uint8)EffectCauserWeight;

	return StrengthMultiplier;
}

float UGEEC_CalculateDamage::GetInstigatorVelocity_MetersPerSec(AActor* InstigatorActor) const
{
	float InstigatorVelocity = 0.f;

	if (IsValid(InstigatorActor) == false)
	{
		return InstigatorVelocity;
	}

	return InstigatorActor->GetVelocity().Length() / 100;
}

float UGEEC_CalculateDamage::GetPenetrationMultiplier(const UDamageSourceData* DamageSourceData, float InstigatorVelocity, USG_AbilitySystemComponent* TargetASC, FVector HitLocation) const
{
	float PenetrationMultiplier = 1.f;

	if (IsValid(TargetASC) == false)
	{
		return PenetrationMultiplier;
	}

	float TargetResistance = 1;
	switch (DamageSourceData->DamageType)
	{
	case ESG_DamageType::Slashing: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceSlashingAttribute()); break;
	case ESG_DamageType::Piercing: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistancePiercingAttribute()); break;
	case ESG_DamageType::Bludgeoning: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceBludgeoningAttribute()); break;
	case ESG_DamageType::Fire: TargetResistance = TargetASC->GetNumericAttribute(USG_AttributeSet::GetResistanceFireAttribute());
	}
	if (TargetResistance <= 0) TargetResistance = 1;

	if (IsTargetGuarding(TargetASC->GetOwner(), TargetASC, HitLocation))
	{
		UE_LOG(DamageLog, Warning, TEXT("The hit is being guarded by the target"));
		TargetResistance *= TargetASC->GetNumericAttribute(USG_AttributeSet::GetGuardProficiencyAttribute());
	}

	PenetrationMultiplier = (DamageSourceData->Penetration + DamageSourceData->Penetration * InstigatorVelocity / 100) / TargetResistance;

	return PenetrationMultiplier;
}

bool UGEEC_CalculateDamage::IsTargetGuarding(AActor* TargetActor, UAbilitySystemComponent* TargetASC, FVector HitLocation) const
{
	AInGameCharacter* TargetCharacter = Cast<AInGameCharacter>(TargetActor);
	if (IsValid(TargetCharacter) == false)
	{
		UE_LOG(DamageLog, Warning, TEXT("Target is not a character"));
		return false;
	}
	if (TargetASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Bool.HasState.Guard"))) == false)
	{
		UE_LOG(DamageLog, Warning, TEXT("Target is not blocking"));
		return false;
	}

	UMeshComponent* TargetMesh = TargetCharacter->GetComponentByClass<UMeshComponent>();
	if (IsValid(TargetMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("UGEEC_CalculateDamage::bIsTargetGuarding IsValid(TargetMesh) == false"));
		return false;
	}

	FVector TargetMeshCenter = TargetMesh->GetLocalBounds().GetBox().GetCenter();

	UE_LOG(DamageLog, Warning, TEXT("Hit point - %s; Target center - %s"), *HitLocation.ToString(), *TargetMeshCenter.ToString());

	if (TargetMeshCenter.Z > HitLocation.Z)
	{
		UE_LOG(DamageLog, Warning, TEXT("The hit is below the blocking range"));
		return false;
	}

	FVector TargetForwardVector = TargetMesh->GetForwardVector();
	FVector HitDirectionVector = UKismetMathLibrary::Normal(HitLocation - TargetActor->GetActorLocation());

	float DotProduct = UKismetMathLibrary::Dot_VectorVector(TargetForwardVector, HitDirectionVector);
	UE_LOG(DamageLog, Warning, TEXT("DotProduct - %f"), DotProduct);

	return (DotProduct > 0);

}
