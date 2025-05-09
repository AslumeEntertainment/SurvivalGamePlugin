// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AttributeSets/SG_CharacterAttributeSet.h"


struct FSG_DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentStrength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentDexterity);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(GuardProficiency);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceSlashing);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistancePiercing);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceBludgeoning);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ResistanceFire);

	FSG_DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, CurrentStrength, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_CharacterAttributeSet, CurrentDexterity, Source, true);

		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, GuardProficiency, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, ResistanceSlashing, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, ResistancePiercing, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, ResistanceBludgeoning, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, ResistanceFire, Target, false);
	}
};

static const FSG_DamageStatics& DamageStatics()
{
	static FSG_DamageStatics DStatics;
	return DStatics;
}
