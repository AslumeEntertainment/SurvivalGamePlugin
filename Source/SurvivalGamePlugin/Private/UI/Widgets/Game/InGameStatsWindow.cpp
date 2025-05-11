// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Game/InGameStatsWindow.h"

#include "Components/TextBlock.h"


void UInGameStatsWindow::SetHealth(float Current, float Max, float Base)
{
	if (!IsValid(TXT_Health) || !IsValid(TXT_BaseHealth))
	{
		return;
	}
	TXT_Health->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseHealth->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetHunger(float Current, float Max, float Base)
{
	if (!IsValid(TXT_Hunger) || !IsValid(TXT_BaseHunger))
	{
		return;
	}
	TXT_Hunger->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseHunger->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetEnergy(float Current, float Max, float Base)
{
	if (!IsValid(TXT_Energy) || !IsValid(TXT_BaseEnergy))
	{
		return;
	}
	TXT_Energy->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseEnergy->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetStrength(float Current, float Base)
{
	if (!IsValid(TXT_Strength) || !IsValid(TXT_BaseStrength))
	{
		return;
	}
	TXT_Strength->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseStrength->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetDexterity(float Current, float Base)
{
	if (!IsValid(TXT_Dexterity) || !IsValid(TXT_BaseDexterity))
	{
		return;
	}
	TXT_Dexterity->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseDexterity->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetSpeed(float Current, float Base)
{
	if (!IsValid(TXT_Speed) || !IsValid(TXT_BaseSpeed))
	{
		return;
	}
	TXT_Speed->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseSpeed->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetAttackSpeed(float Current, float Base)
{
	if (!IsValid(TXT_AttackSpeed) || !IsValid(TXT_BaseAttackSpeed))
	{
		return;
	}
	TXT_AttackSpeed->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseAttackSpeed->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetJumpVelocity(float Current, float Base)
{
	if (!IsValid(TXT_JumpVelocity) || !IsValid(TXT_BaseJumpVelocity))
	{
		return;
	}
	TXT_JumpVelocity->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseJumpVelocity->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetSlashingDefence(float Current, float Base)
{
	if (!IsValid(TXT_SlashingDefence) || !IsValid(TXT_BaseSlashingDefence))
	{
		return;
	}
	TXT_SlashingDefence->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseSlashingDefence->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetBleedImmunity(bool Value)
{
	TXT_BleedImmunity->SetRenderOpacity(Value ? 1.f : 0.f);
}

void UInGameStatsWindow::SetPiercingDefence(float Current, float Base)
{
	if (!IsValid(TXT_PiercingDefence) || !IsValid(TXT_BasePiercingDefence))
	{
		return;
	}
	TXT_PiercingDefence->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BasePiercingDefence->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetImpaleImmunity(bool Value)
{
	TXT_ImpaleImmunity->SetRenderOpacity(Value ? 1.f : 0.f);
}

void UInGameStatsWindow::SetBludgeoningDefence(float Current, float Base)
{
	if (!IsValid(TXT_BludgeoningDefence) || !IsValid(TXT_BaseBludgeoningDefence))
	{
		return;
	}
	TXT_BludgeoningDefence->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseBludgeoningDefence->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetKnockbackImmunity(bool Value)
{
	TXT_KnockbackImmunity->SetRenderOpacity(Value ? 1.f : 0.f);
}

void UInGameStatsWindow::SetFireDefence(float Current, float Base)
{
	if (!IsValid(TXT_FireDefence) || !IsValid(TXT_BaseFireDefence))
	{
		return;
	}
	TXT_FireDefence->SetText(FText::FromString(FString::Printf(TEXT("%.f"), Current)));
	TXT_BaseFireDefence->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetBurningImmunity(bool Value)
{
	TXT_BurningImmunity->SetRenderOpacity(Value ? 1.f : 0.f);
}

void UInGameStatsWindow::SetGuardProficiency(float Current, float Max, float Base)
{
	if (!IsValid(TXT_GuardProficiency) || !IsValid(TXT_BaseGuardProficiency))
	{
		return;
	}
	TXT_GuardProficiency->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseGuardProficiency->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}