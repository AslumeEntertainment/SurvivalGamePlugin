// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"

#include "Headers/AbilityInputConfig.h"

#include "SG_EnhancedInputComponent.generated.h"


UCLASS()
class SURVIVALGAMEPLUGIN_API USG_EnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(UAbilityInputConfig* AbilityInputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);

};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
inline void USG_EnhancedInputComponent::BindAbilityActions(UAbilityInputConfig* AbilityInputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	check(AbilityInputConfig);

	for (FAbilityInputData& AbilityInputAction : AbilityInputConfig->AbilityInputActions)
	{
		if (IsValid(AbilityInputAction.InputAction) == false || AbilityInputAction.InputTag.IsValid() == false)
		{
			continue;
		}

		if (PressedFunc)
		{
			BindAction(AbilityInputAction.InputAction, ETriggerEvent::Started, Object, PressedFunc, AbilityInputAction.InputTag, AbilityInputAction.InputAction);
		}
		if (ReleasedFunc)
		{
			BindAction(AbilityInputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, AbilityInputAction.InputTag, AbilityInputAction.InputAction);
		}
	}
}
