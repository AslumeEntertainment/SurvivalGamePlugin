// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/SG_AnimNotify_ReportSoundEvent.h"

#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/SG_Character.h"


void USG_AnimNotify_ReportSoundEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (IsValid(Sound) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_ReportSoundEvent::Notify IsValid(Sound) == false"));
		return;
	}

	UWorld* World = MeshComp->GetWorld();
	if (IsValid(World) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_ReportSoundEvent::Notify IsValid(World) == false"));
		return;
	}

#if WITH_EDITORONLY_DATA
	if (World->WorldType == EWorldType::EditorPreview)
	{
		if (MeshComp->IsPlaying())
		{
			UGameplayStatics::PlaySound2D(World, Sound, Volume, 1);
		}
		return;
	}
#endif

	ASG_Character* Character = Cast<ASG_Character>(MeshComp->GetOwner());
	if (IsValid(Character) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("USG_AnimNotify_ReportSoundEvent::Notify IsValid(Character) == false"));
		return;
	}

	float MaxRange = DefaultMaxRange;
	if (IsValid(Sound->AttenuationSettings) && bOverrideAttenuationRangeIfSet)
	{
		MaxRange = Sound->AttenuationSettings->Attenuation.FalloffDistance;
	}
	
	UGameplayStatics::PlaySoundAtLocation(World, Sound, Character->GetActorLocation(), Volume);
	UAISense_Hearing::ReportNoiseEvent(World, Character->GetActorLocation(), Loudness, Character, MaxRange);
}
