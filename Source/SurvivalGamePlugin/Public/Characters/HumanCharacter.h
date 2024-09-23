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
	
private:

	UPROPERTY(Replicated)
	UObject* FocusedInteractableObject = nullptr;

	UFUNCTION(Server, Reliable)
	void Server_SetFocusedInteractable(UObject* InFocusedInteractable);

	void EmitInteractionChecker();
	void CheckForInteractables(FHitResult HitResult);

protected:

	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	UMaterialInterface* InteractableOverlayMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CraftingWindowOnlyMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* UnequipAction;

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

	void AttemptInteraction();
	void StartUnequipItem();
	void CancelUnequipItem();

	void AttemptUseMainHandItem();
	void AttemptUseSecondaryHandItem();

	UFUNCTION(Server, Reliable)
	void Server_ContinueUnequipMainHandItem();
	UFUNCTION(Server, Reliable)
	void Server_ContinueUnequipSecondaryHandItem();

	void UseAbility();

	void OpenCraftingWindow();
	void CloseCraftingWindow();

	void AttemptDisassembleItem();

public:

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShouldDrawDebug = true;

	UPROPERTY(Replicated)
	AItemBase* MainHandItem;
	UPROPERTY(Replicated)
	AItemBase* SecondaryHandItem;
	UPROPERTY(Replicated, BlueprintReadOnly)
	AItemBase* ActiveItem;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	bool bIsRightHanded = true;

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
