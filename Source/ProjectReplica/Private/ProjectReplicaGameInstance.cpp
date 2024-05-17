// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectReplicaGameInstance.h"
#include "Weapons/PRBaseWeapon.h"

UProjectReplicaGameInstance::UProjectReplicaGameInstance()
{
	// ElementColor
	ElementColorDataTable = nullptr;
	ElementColors.Empty();
}

void UProjectReplicaGameInstance::Init()
{
	Super::Init();

	// ElementColor
	InitializeElementColors();

	// WeaponInfo
	InitializeWeaponInfo();
}

#pragma region ElementColor
FLinearColor UProjectReplicaGameInstance::GetElementColor(EPRElementType ElementType) const
{
	if(ElementColors.Contains(ElementType))
	{
		return *ElementColors.Find(ElementType);
	}

	// 존재하지 않는 속성일 경우 물리 속성의 색상을 반환합니다.
	return *ElementColors.Find(EPRElementType::ElementType_Physio);
}

void UProjectReplicaGameInstance::InitializeElementColors()
{
	ElementColors.Empty();
	
	if(ElementColorDataTable)
	{
		TArray<FName> RowNames = ElementColorDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRElementColor* DataTableRow = ElementColorDataTable->FindRow<FPRElementColor>(RowName, FString(""));
			if(DataTableRow)
			{
				ElementColors.Emplace(DataTableRow->ElementType, DataTableRow->Color);
			}
		}
	}
}
#pragma endregion 

#pragma region WeaponInfo
FPRWeaponStat UProjectReplicaGameInstance::GetWeaponStat(TSubclassOf<APRBaseWeapon> NewWeapon) const
{
	if(WeaponInfos.Contains(NewWeapon))
	{
		return *WeaponInfos.Find(NewWeapon);
	}
	
	return FPRWeaponStat();
}

void UProjectReplicaGameInstance::InitializeWeaponInfo()
{
	WeaponInfos.Empty();
	
	if(WeaponInfoDataTable)
	{
		TArray<FName> RowNames = WeaponInfoDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRWeaponInfo* DataTableRow = WeaponInfoDataTable->FindRow<FPRWeaponInfo>(RowName, FString(""));
			if(DataTableRow)
			{
				WeaponInfos.Emplace(DataTableRow->WeaponClass, DataTableRow->WeaponStat);
			}
		}
	}
}

TMap<TSubclassOf<APRBaseWeapon>, FPRWeaponStat> UProjectReplicaGameInstance::GetWeaponInfos() const
{
	return WeaponInfos;
}
#pragma endregion 
