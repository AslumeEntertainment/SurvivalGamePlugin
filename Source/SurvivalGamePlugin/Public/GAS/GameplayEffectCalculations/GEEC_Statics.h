// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/AttributeSets/SG_AttributeSet.h"
//#include "GEEC_Statics.generated.h"


struct FSG_DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	FSG_DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USG_AttributeSet, Health, Target, false);
	}
};

static const FSG_DamageStatics& DamageStatics()
{
	static FSG_DamageStatics DStatics;
	return DStatics;
}
