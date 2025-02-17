// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DamageFunctionLibrary.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API UDamageFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static void ShowDamageIndicator(UWorld* World, float Damage, AActor* HitActor);

};
