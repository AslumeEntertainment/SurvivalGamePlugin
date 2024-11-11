// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Abilities/SGGA_ApplyEffectOnContact.h"

#include "Engine/StaticMeshActor.h"

#include "Components/HittableComponent.h"
#include "GAS/SG_AbilitySystemBlueprintLibrary.h"
#include "GAS/SG_AbilitySystemComponent.h"


void USGGA_ApplyEffectOnContact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility CommitAbility == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AvatarActor = ActorInfo->AvatarActor.Get();
	if (IsValid(AvatarActor) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(AvatarActor) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UHittableComponent* HittableComp = ActorInfo->AvatarActor.Get()->GetComponentByClass<UHittableComponent>();
	if (IsValid(HittableComp) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::ActivateAbility IsValid(HittableComp) == false"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (OverlayMeshActorClass != nullptr)
	{
		SpawnOverlayMesh();
	}

	HittableComp->OnComponentBeginOverlap.AddDynamic(this, &USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap);
	if (bShouldApplyOnActivation == false)
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	HittableComp->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor == AvatarActor && bShouldAffectUser == false)
		{
			continue;
		}
		AttemptHit(Actor);
	}
}

void USGGA_ApplyEffectOnContact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(OverlayMeshActor))
	{
		OverlayMeshActor->Destroy();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USGGA_ApplyEffectOnContact::SpawnOverlayMesh()
{
	if (HasAuthority(&CurrentActivationInfo) == false)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::SpawnOverlayMesh IsValid(World) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	OverlayMeshActor = World->SpawnActor<AStaticMeshActor>(OverlayMeshActorClass, AvatarActor->GetActorLocation(), AvatarActor->GetActorRotation());

	UMeshComponent* AvatarMesh = AvatarActor->GetComponentByClass<UMeshComponent>();
	if (IsValid(AvatarMesh) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::SpawnOverlayMesh IsValid(AvatarMesh) == false"));
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	OverlayMeshActor->AttachToComponent(AvatarMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketToAttach);
}

void USGGA_ApplyEffectOnContact::AttemptHit(AActor* TargetActor)
{
	UHittableComponent* TargetHittableComp = TargetActor->GetComponentByClass<UHittableComponent>();
	if (IsValid(TargetHittableComp) == false)
	{
		return;
	}
	if (TargetHittableComp->CanEverApplyGameplayEffects() == false)
	{
		return;
	}

	USG_AbilitySystemComponent* ASC = TargetHittableComp->GetOwnerAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USGGA_ApplyEffectOnContact::AttemptHit IsValid(ASC) == false"));
		return;
	}
	for (TSubclassOf<UGameplayEffect> Effect : EffectsToApply)
	{
		ASC->AddEffectFromClass(Effect, AvatarActor);
	}
}

void USGGA_ApplyEffectOnContact::OnAvatarActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AttemptHit(OtherActor);
}

