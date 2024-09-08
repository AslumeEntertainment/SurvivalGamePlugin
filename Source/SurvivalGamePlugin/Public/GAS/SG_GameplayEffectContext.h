// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "Headers/DamageGameplayEffectStruct.h"

#include "SG_GameplayEffectContext.generated.h"


USTRUCT()
struct SURVIVALGAMEPLUGIN_API FSG_GameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_USTRUCT_BODY()

protected:

	UPROPERTY()
	ESG_DamageType DamageType;

public:

	virtual ESG_DamageType GetDamageType()
	{
		return DamageType;
	}

	virtual void SetDamageType(const ESG_DamageType InDamageType)
	{
		DamageType = InDamageType;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSG_GameplayEffectContext::StaticStruct();
	}

	virtual FSG_GameplayEffectContext* Duplicate() const override
	{
		FSG_GameplayEffectContext* NewContext = new FSG_GameplayEffectContext();
		*NewContext = *this;
		NewContext->AddActors(Actors);

		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}

		NewContext->DamageType = DamageType;
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override
	{
		uint8 RepBits = 0;
		if (Ar.IsSaving())
		{
			if (bReplicateInstigator && Instigator.IsValid())
			{
				RepBits |= 1 << 0;
			}
			if (bReplicateEffectCauser && EffectCauser.IsValid())
			{
				RepBits |= 1 << 1;
			}
			if (AbilityCDO.IsValid())
			{
				RepBits |= 1 << 2;
			}
			if (bReplicateSourceObject && SourceObject.IsValid())
			{
				RepBits |= 1 << 3;
			}
			if (Actors.Num() > 0)
			{
				RepBits |= 1 << 4;
			}
			if (HitResult.IsValid())
			{
				RepBits |= 1 << 5;
			}
			if (bHasWorldOrigin)
			{
				RepBits |= 1 << 6;
			}
			if (DamageType != ESG_DamageType::None)
			{
				RepBits |= 1 << 7;
			}
		}

		Ar.SerializeBits(&RepBits, 8);

		if (RepBits & (1 << 0))
		{
			Ar << Instigator;
		}
		if (RepBits & (1 << 1))
		{
			Ar << EffectCauser;
		}
		if (RepBits & (1 << 2))
		{
			Ar << AbilityCDO;
		}
		if (RepBits & (1 << 3))
		{
			Ar << SourceObject;
		}
		if (RepBits & (1 << 4))
		{
			SafeNetSerializeTArray_Default<31>(Ar, Actors);
		}
		if (RepBits & (1 << 5))
		{
			if (Ar.IsLoading())
			{
				if (!HitResult.IsValid())
				{
					HitResult = TSharedPtr<FHitResult>(new FHitResult());
				}
			}
			HitResult->NetSerialize(Ar, Map, bOutSuccess);
		}
		if (RepBits & (1 << 6))
		{
			Ar << WorldOrigin;
			bHasWorldOrigin = true;
		}
		if (RepBits & (1 << 7))
		{
			Ar << DamageType;
		}
		else
		{
			bHasWorldOrigin = false;
		}

		if (Ar.IsLoading())
		{
			AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
		}

		bOutSuccess = true;
		return true;
	}
};

template <>
struct TStructOpsTypeTraits<FSG_GameplayEffectContext> : public TStructOpsTypeTraitsBase2<FSG_GameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};