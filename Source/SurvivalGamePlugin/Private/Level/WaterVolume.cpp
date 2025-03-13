// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/WaterVolume.h"

#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/FluidDataComponent.h"
#include "Components/BoxComponent.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


AWaterVolume::AWaterVolume()
{
	WaterBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterBox"));
	WaterBox->SetupAttachment(GetRootComponent());

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(WaterBox);
	PostProcessComponent->bUnbound = false;
	PostProcessComponent->bUseAttachParentBound = true;

	WaterSurfaceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterSurfaceMesh"));
	WaterSurfaceMesh->SetupAttachment(WaterBox);
	WaterSurfaceMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FluidDataComponent = CreateDefaultSubobject<UFluidDataComponent>(TEXT("FluidData"));

	OnActorBeginOverlap.AddDynamic(this, &AWaterVolume::OnActorEnteringVolume);
	OnActorEndOverlap.AddDynamic(this, &AWaterVolume::OnActorLeavingVolume);
}

void AWaterVolume::OnActorEnteringVolume(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsValid(OtherActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(Other) == false"));
		return;
	}
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(ASC) == false"));
		return;
	}

	ASC->AddLooseGameplayTags(TagsToApplyWhenEntered);

	for (TSubclassOf<UGameplayEffect> Effect : EffectsToApplyWhenEntered)
	{
		if (Effect != nullptr)
		{
			ASC->AddEffectFromClass(Effect, this, nullptr, nullptr);
		}
	}
}

void AWaterVolume::OnActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsValid(OtherActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(Other) == false"));
		return;
	}
	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(ASC) == false"));
		return;
	}

	ASC->RemoveLooseGameplayTags(TagsToApplyWhenEntered);
}


