// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Game/InGameStatsWindow.h"

#include "Components/TextBlock.h"

void UInGameStatsWindow::SetHealth(float Current, float Max, float Base)
{
	TXT_Health->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseHealth->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetHunger(float Current, float Max, float Base)
{
	TXT_Hunger->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseHunger->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetEnergy(float Current, float Max, float Base)
{
	TXT_Energy->SetText(FText::FromString(FString::Printf(TEXT("%.f/%.f"), Current, Max)));
	TXT_BaseEnergy->SetText(FText::FromString(FString::Printf(TEXT("(%.f)"), Base)));
}

void UInGameStatsWindow::SetStrength(float Current, float Base)
{
}

void UInGameStatsWindow::SetDexterity(float Current, float Base)
{
}

void UInGameStatsWindow::SetSpeed(float Current, float Base)
{
}

void UInGameStatsWindow::SetAttackSpeed(float Current, float Base)
{
}

void UInGameStatsWindow::SetJumpVelocity(float Current, float Base)
{
}

void UInGameStatsWindow::SetSlashingDefence(float Current, float Base)
{
}

void UInGameStatsWindow::SetBleedImmunity(bool Value)
{
}

void UInGameStatsWindow::SetPiercingDefence(float Current, float Base)
{
}

void UInGameStatsWindow::SetImpaleImmunity(bool Value)
{
}

void UInGameStatsWindow::SetBludgeoningDefence(float Current, float Base)
{
}

void UInGameStatsWindow::SetKnockbackImmunity(bool Value)
{
}

void UInGameStatsWindow::SetFireDefence(float Current, float Base)
{
}

void UInGameStatsWindow::SetBurningImmunity(bool Value)
{
}

void UInGameStatsWindow::SetGuardProficiency(float Current, float Base)
{
}