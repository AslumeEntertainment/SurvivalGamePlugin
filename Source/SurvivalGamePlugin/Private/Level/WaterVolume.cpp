// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/WaterVolume.h"

#include "Components/PostProcessComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/FluidDataComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


AWaterVolume::AWaterVolume()
{
	WaterBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WaterBox"));
	SetRootComponent(WaterBox);

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

float AWaterVolume::GetSurfaceZ()
{
	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent);

	return Origin.Z + BoxExtent.Z;
}

void AWaterVolume::OnActorEnteringVolume(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsValid(OtherActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(OtherActor) == false"));
		return;
	}

	MakeSplash(OtherActor->GetTransform());

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(ASC) == false"));
		return;
	}
	if (IsValid(EffectToApplyWhileInWater))
	{
		ASC->AddEffectFromClass(EffectToApplyWhileInWater, this, nullptr, nullptr);
	}

	GetSurfaceZ();
}

void AWaterVolume::OnActorLeavingVolume(AActor* OverlappedActor, AActor* OtherActor)
{
	if (IsValid(OtherActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(Other) == false"));
		return;
	}

	MakeSplash(OtherActor->GetTransform());

	USG_AbilitySystemComponent* ASC = Cast<USG_AbilitySystemComponent>(USG_AbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::ActorEnteredVolume IsValid(ASC) == false"));
		return;
	}

	ASC->RemoveActiveEffectsFromClass(EffectToApplyWhileInWater);
}

void AWaterVolume::MakeSplash(FTransform SplashTransform)
{
	if (IsValid(OnWaterEnteredEffect) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWaterVolume::MakeSplash IsValid(OnWaterEnteredEffect) == false"));
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AWaterVolume::MakeSplash IsValid(World) == false"));
		return;
	}

	FVector EffectLocation = SplashTransform.GetLocation();
	EffectLocation.Z = GetSurfaceZ();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, OnWaterEnteredEffect, EffectLocation,
		SplashTransform.Rotator(), SplashTransform.GetScale3D());

}


