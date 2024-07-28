// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SG_Character.h"
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
class SURVIVALGAMEPLUGIN_API AHumanCharacter : public ASG_Character
{
	GENERATED_BODY()
	
private:

	bool bIsCraftingWindowOpen = false;

	UObject* FocusedInteractableObject = nullptr;

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);

protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FName LeftHandSocket;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
	FName RightHandSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CraftingWindowOnlyMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipMainHandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryHandAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseMainHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UseSecondaryHandItemAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* AbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* CraftingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* DisassembleAction;

	void AttemptEquipWithMainHand();
	void AttemptEquipWithSecondaryHand();

	void AttemptUseMainHandItem();
	void AttemptUseSecondaryHandItem();

	void UseAbility();

	void OpenCraftingWindow();
	void AttemptDisassembleItem();

public:

	UPROPERTY(Replicated)
	AItemBase* MainHandItem;
	UPROPERTY(Replicated)
	AItemBase* SecondaryHandItem;

	UFUNCTION()
	void CraftItem(FText ProductItemName);
	void SwitchInputMappingContext(EInputMappingContextState InputContextState);

	UFUNCTION(BlueprintCallable)
	FName GetMainHandSocket();
	UFUNCTION(BlueprintCallable)
	FName GetSecondaryHandSocket();

	UFUNCTION(BlueprintCallable)
	UObject* GetFocusedInteractableObject();

	UFUNCTION(BlueprintCallable)
	void ShowPlayerInfo();

	FCraftingNotifySignature CraftingWindowOpenDelegate;
	FUINotifySignature CraftingWindowCloseDelegate;

};
