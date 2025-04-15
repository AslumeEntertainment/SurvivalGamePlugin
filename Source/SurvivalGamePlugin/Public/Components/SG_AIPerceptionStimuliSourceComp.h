// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

#include "GameplayTagContainer.h"

#include "SG_AIPerceptionStimuliSourceComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SURVIVALGAMEPLUGIN_API USG_AIPerceptionStimuliSourceComp : public UAIPerceptionStimuliSourceComponent
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", meta = (Categories = "StimuliSource"))
	FGameplayTag StimuliSourceTag;

public:

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetStimuliSourceTag();

};
