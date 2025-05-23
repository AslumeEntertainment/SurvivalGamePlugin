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

	/*AActor* MyParent = GetParentActor();
	if (IsValid(MyParent) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("AGC_Burning::OnActive_Implementation IsValid(MyParent) == false"));
		return;
	}

	FVector Origin;
	FVector BoxExtent;
	MyParent->GetActorBounds(true, Origin, BoxExtent);

	FireNiagaraComponent->SetWorldScale3D(BoxExtent * 20);*/
}

bool AGC_Burning::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	

	/*FVector MinLocalBounds = ParentRoot->GetLocalBounds().GetBox().Min;
	FVector MaxLocalBounds = ParentRoot->GetLocalBounds().GetBox().Max;

	FVector ActorBottom = MyParent->GetActorLocation() + MyParent->GetActorUpVector() * MinLocalBounds.Z;
	FVector ActorTop = MyParent->GetActorLocation() + MyParent->GetActorUpVector() * MaxLocalBounds.Z;

	FVector TraceCenter = (ActorTop + ActorBottom) / 2;
	FVector TraceExtent = ParentRoot->GetLocalBounds().GetBox().GetExtent();
	FVector Volume = TraceExtent * 2;

	FireNiagaraComponent->SetRelativeScale3D(Volume / 100);

	UE_LOG(LogTemp, Warning, TEXT("Volume - %s"), *Volume.ToString());*/

	FireNiagaraComponent->Activate();

	return true;
}

bool AGC_Burning::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	FireNiagaraComponent->Deactivate();

	return true;
}
