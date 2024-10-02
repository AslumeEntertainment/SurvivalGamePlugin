// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/InGameCharacter.h"
#include "Headers/ItemDataStruct.h"
#include "HumanCharacter.generated.h"

class AItemBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCraftingNotifySignature, FItemDataStruct, MainItemData, FName, IngredientItemName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUINotifySignature);

UENUM(BlueprintType)
enum class EInputMappingContextState : uint8
{
	Default,
	Crafting
};

UCLASS()
class SURVIVALGAMEPLUGIN_API AHumanCharacter : public AInGameCharacter
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CraftingWindowOnlyMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CraftingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DisassembleAction;

	void MainHandAction() override;
	void SecondaryHandAction() override;

	void UnequipItem() override;
	void CancelUnequipItem();

	void UseDebugAbility();

	void OpenCraftingWindow();
	void CloseCraftingWindow();

	void AttemptDisassembleItem();

public:

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

	UFUNCTION()
	void CraftItem(FText ProductItemName);
	void SwitchInputMappingContext(EInputMappingContextState InputContextState);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerInfo();

	FCraftingNotifySignature CraftingWindowOpenDelegate;
	FUINotifySignature CraftingWindowCloseDelegate;

};
