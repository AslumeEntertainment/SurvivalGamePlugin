// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "IdSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SURVIVALGAMEPLUGIN_API UIdSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:


	void Initialize(FSubsystemCollectionBase& Collection) override;

	void Deinitialize() override;

	bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:

	UPROPERTY(EditDefaultsOnly)
	TMap<uint16, FDataTableRowHandle> IDMap;

public:

	FDataTableRowHandle GetDataRowFromID(uint16 ID);

	template <class T>
	T* GetDataFromID(uint16 ID);

	uint16 GetIdFromDataRow(FDataTableRowHandle RowHandle);

};

template<class T>
inline T* UIdSubsystem::GetDataFromID(uint16 ID)
{
	return GetDataRowFromID(ID).GetRow<T>();
}
