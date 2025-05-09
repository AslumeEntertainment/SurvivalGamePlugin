// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"


struct FSG_DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentStrength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentDexterity);

	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentGuardProficiency);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentSlashingDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentPiercingDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentBludgeoningDefence);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentFireDefence);

	FSG_DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, CurrentStrength, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, CurrentDexterity, Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentHealth, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentGuardProficiency, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentSlashingDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentPiercingDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentBludgeoningDefence, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, CurrentFireDefence, Target, false);
	}
};

static const FSG_DamageStatics& DamageStatics()
{
	static FSG_DamageStatics DStatics;
	return DStatics;
}
