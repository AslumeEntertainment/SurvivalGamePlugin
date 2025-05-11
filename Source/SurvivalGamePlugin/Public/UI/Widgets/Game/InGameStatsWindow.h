// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameStatsWindow.generated.h"

class UTextBlock;

UCLASS()
class SURVIVALGAMEPLUGIN_API UInGameStatsWindow : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Health;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseHealth;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Hunger;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseHunger;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Energy;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseEnergy;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Strength;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseStrength;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Dexterity;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseDexterity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_Speed;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseSpeed;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_AttackSpeed;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseAttackSpeed;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_JumpVelocity;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseJumpVelocity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_SlashingDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseSlashingDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BleedImmunity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_PiercingDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BasePiercingDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_ImpaleImmunity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BludgeoningDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseBludgeoningDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_KnockbackImmunity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_FireDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseFireDefence;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BurningImmunity;

	//----------------------------------------------

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_GuardProficiency;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* TXT_BaseGuardProficiency;

	//----------------------------------------------

public:

	UFUNCTION()
	void SetHealth(float Current, float Max, float Base);

	UFUNCTION()
	void SetHunger(float Current, float Max, float Base);

	UFUNCTION()
	void SetEnergy(float Current, float Max, float Base);

	UFUNCTION()
	void SetStrength(float Current, float Base);

	UFUNCTION()
	void SetDexterity(float Current, float Base);

	UFUNCTION()
	void SetSpeed(float Current, float Base);

	UFUNCTION()
	void SetAttackSpeed(float Current, float Base);

	UFUNCTION()
	void SetJumpVelocity(float Current, float Base);

	UFUNCTION()
	void SetSlashingDefence(float Current, float Base);

	UFUNCTION()
	void SetBleedImmunity(bool Value);

	UFUNCTION()
	void SetPiercingDefence(float Current, float Base);

	UFUNCTION()
	void SetImpaleImmunity(bool Value);

	UFUNCTION()
	void SetBludgeoningDefence(float Current, float Base);

	UFUNCTION()
	void SetKnockbackImmunity(bool Value);

	UFUNCTION()
	void SetFireDefence(float Current, float Base);

	UFUNCTION()
	void SetBurningImmunity(bool Value);

	UFUNCTION()
	void SetGuardProficiency(float Current, float Max, float Base);
};
