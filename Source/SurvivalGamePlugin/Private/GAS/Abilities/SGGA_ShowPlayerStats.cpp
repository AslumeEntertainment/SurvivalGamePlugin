// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ShowPlayerStats.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GameFramework/PlayerController.h"

#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"
#include "GAS/SG_AbilitySystemComponent.h"
#include "UI/Widgets/Game/InGameStatsWindow.h"
#include "Characters/InGameCharacter.h"


void USGGA_ShowPlayerStats::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (IsValid(InGameStatsWindowClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(InGameStatsWindowClass) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AInGameCharacter* Player = Cast<AInGameCharacter>(ActorInfo->AvatarActor.Get());
	if (IsValid(Player) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(Player) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	APlayerController* PlayerCont = Cast<APlayerController>(Player->GetController());
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(PlayerCont) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	ASC = Player->GetSG_AbilitySystemComponent();
	if (IsValid(PlayerCont) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ShowPlayerStats::ActivateAbility IsValid(ASC) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const USG_CharacterAttributeSet* AttributeSet = Cast<USG_CharacterAttributeSet>(ASC->GetAttributeSet(USG_CharacterAttributeSet::StaticClass()));

	InGameStatsWindow = CreateWidget<UInGameStatsWindow>(PlayerCont, InGameStatsWindowClass);

	InGameStatsWindow->SetHealth(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentHealthAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHealthAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseHealthAttribute()));

	InGameStatsWindow->SetHunger(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetHungerAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHungerAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHungerAttribute()));

	InGameStatsWindow->SetEnergy(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetEnergyAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxEnergyAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxEnergyAttribute()));

	InGameStatsWindow->SetStrength(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentStrengthAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseStrengthAttribute()));

	InGameStatsWindow->SetDexterity(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentDexterityAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseDexterityAttribute()));

	InGameStatsWindow->SetSpeed(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentSpeedAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseSpeedAttribute()));

	InGameStatsWindow->SetAttackSpeed(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentAttackSpeedPercentAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseAttackSpeedPercentAttribute()));

	InGameStatsWindow->SetJumpVelocity(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentJumpVelocityAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseJumpVelocityAttribute()));

	InGameStatsWindow->SetSlashingDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentSlashingDefenceAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseSlashingDefenceAttribute()));

	InGameStatsWindow->SetPiercingDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentPiercingDefenceAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBasePiercingDefenceAttribute()));

	InGameStatsWindow->SetBludgeoningDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentBludgeoningDefenceAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseBludgeoningDefenceAttribute()));

	InGameStatsWindow->SetFireDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentFireDefenceAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseFireDefenceAttribute()));

	InGameStatsWindow->SetGuardProficiency(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentGuardProficiencyAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxGuardProficiencyAttribute()),
		ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseGuardProficiencyAttribute()));

	InGameStatsWindow->SetBleedImmunity(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.Bleed")));

	InGameStatsWindow->SetImpaleImmunity(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.Impale")));

	InGameStatsWindow->SetKnockbackImmunity(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.Knockback")));

	InGameStatsWindow->SetBurningImmunity(ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Bool.HasImmunity.Burning")));

	/*
	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetHealth(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHealthAttribute()),
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseHealthAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetHealth(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentHealthAttribute()),
					Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseHealthAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetHealth(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentHealthAttribute()),
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHealthAttribute()),
					Data.NewValue);
			}
		}
	);
	*/
	//-------------------------------------------------------------------------------------------------
	/*
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			SetHunger(Data.NewValue,
				ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHungerAttribute()),
				ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxHungerAttribute()));
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHungerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			SetHunger(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetHungerAttribute()),
				Data.NewValue,
				Data.NewValue);
		}
	);
	*/
	/*
	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetEnergy(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxEnergyAttribute()),
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxEnergyAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxEnergyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetEnergy(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetEnergyAttribute()),
					Data.NewValue,
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentStrengthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetStrength(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseStrengthAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseStrengthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetStrength(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentStrengthAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentDexterityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetDexterity(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseDexterityAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseDexterityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetDexterity(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentDexterityAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetSpeed(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseSpeedAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetSpeed(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentSpeedAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentAttackSpeedPercentAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetAttackSpeed(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseAttackSpeedPercentAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseAttackSpeedPercentAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetAttackSpeed(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentAttackSpeedPercentAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentJumpVelocityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetJumpVelocity(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseJumpVelocityAttribute()));
			}	
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseJumpVelocityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetJumpVelocity(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentJumpVelocityAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentSlashingDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetSlashingDefence(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseSlashingDefenceAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseSlashingDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetSlashingDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentSlashingDefenceAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentPiercingDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetPiercingDefence(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBasePiercingDefenceAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBasePiercingDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetPiercingDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentPiercingDefenceAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentBludgeoningDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetBludgeoningDefence(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseBludgeoningDefenceAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseBludgeoningDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetBludgeoningDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentBludgeoningDefenceAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentFireDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetFireDefence(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseFireDefenceAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseFireDefenceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetFireDefence(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentFireDefenceAttribute()),
					Data.NewValue);
			}
		}
	);

	//-------------------------------------------------------------------------------------------------

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetCurrentGuardProficiencyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetGuardProficiency(Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxGuardProficiencyAttribute()),
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseGuardProficiencyAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxGuardProficiencyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetGuardProficiency(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentGuardProficiencyAttribute()),
					Data.NewValue,
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetBaseGuardProficiencyAttribute()));
			}
		}
	);

	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetBaseGuardProficiencyAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			if (IsValid(InGameStatsWindow))
			{
				InGameStatsWindow->SetGuardProficiency(ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetCurrentGuardProficiencyAttribute()),
					ASC->GetNumericAttribute(USG_CharacterAttributeSet::GetMaxGuardProficiencyAttribute()),
					Data.NewValue);
			}
		}
	);
	*/

	InGameStatsWindow->AddToViewport();

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &USGGA_ShowPlayerStats::CloseWindow);
	WaitInputReleaseTask->ReadyForActivation();
}

void USGGA_ShowPlayerStats::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	const USG_CharacterAttributeSet* AttributeSet = Cast<USG_CharacterAttributeSet>(ASC->GetAttributeSet(USG_CharacterAttributeSet::StaticClass()));
	
	InGameStatsWindow->RemoveFromParent();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_ShowPlayerStats::CloseWindow(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USGGA_ShowPlayerStats::SetHunger(float Current, float Max, float Base)
{
	if (IsValid(InGameStatsWindow) == false)
	{
		return;
	}
	InGameStatsWindow->SetHunger(Current, Max, Base);
}

