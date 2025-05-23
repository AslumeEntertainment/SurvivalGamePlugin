// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GameplayCues/GC_Burning.h"

#include "NiagaraComponent.h"

AGC_Burning::AGC_Burning()
{
	PrimaryActorTick.bCanEverTick = false;
	bAutoDestroyOnRemove = true;
	bAutoAttachToOwner = true;
	AutoDestroyDelay = 3.f;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComponent);

	FireNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fire"));
	FireNiagaraComponent->SetupAttachment(SceneComponent);
	
}

void AGC_Burning::BeginPlay()
{
	Super::BeginPlay();
}

bool AGC_Burning::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	FireNiagaraComponent->Activate();

	return true;
}

bool AGC_Burning::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	FireNiagaraComponent->Deactivate();

	return true;
}
