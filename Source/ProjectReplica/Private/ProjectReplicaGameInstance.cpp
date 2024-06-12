// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectReplicaGameInstance.h"
#include "Weapons/PRBaseWeapon.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"

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

	// CharacterStatInfo
	InitializeCharacterStatSettings();
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

#pragma region CharacterStatInfo
FPRCharacterStat UProjectReplicaGameInstance::GetCharacterStat(TSubclassOf<APRBaseCharacter> NewCharacter, int32 NewLevel) const
{
	// 일치하는 항목을 찾지 못했을 때 반활할 기본 능력치
	FPRCharacterStat DefaultStat;

	// CharacterStatSettings에 NewCharacter에 대한 항목이 있는지 확인합니다.
	if(CharacterStatSettings.Contains(NewCharacter))
	{
		// NewCharacter에 대한 FPRLevelToCharacterStat을 가져옵니다.
		const FPRLevelToCharacterStat& LevelToStat = *CharacterStatSettings.Find(NewCharacter);

		// NewLevel보다 작거나 같은 가장 가까운 Level 키를 찾습니다.
		int32 ClosestLevel = -1;
		
		for(auto& Pair : LevelToStat.CharacterStats)
		{
			int32 LevelKey = Pair.Key;
				
			// LevelKey가 NewLevel보다 작거나 같고, 현재 ClosestLevel보다 큰 경우
			if(LevelKey <= NewLevel && LevelKey > ClosestLevel)
			{
				ClosestLevel = LevelKey;
			}
		}

		// 유효한 ClosestLevel을 찾은 경우, 해당 CharacterStat을 반환합니다.
		if(ClosestLevel != -1 && LevelToStat.CharacterStats.Contains(ClosestLevel))
		{
			return *LevelToStat.CharacterStats.Find(ClosestLevel);
		}
	}

	// 일치하는 항목을 찾지 못한 경우, 기본 능력치를 반환합니다.
	return DefaultStat;
}

void UProjectReplicaGameInstance::InitializeCharacterStatSettings()
{
	CharacterStatSettings.Empty();

	if(CharacterStatSettingsDataTable)
	{
		TArray<FName> RowNames = CharacterStatSettingsDataTable->GetRowNames();
		for(const auto& RowName : RowNames)
		{
			FPRLevelToCharacterStatSettings* DataTableRow = CharacterStatSettingsDataTable->FindRow<FPRLevelToCharacterStatSettings>(RowName, FString(""));
			if(DataTableRow)
			{
				CharacterStatSettings.Emplace(DataTableRow->CharacterClass, DataTableRow->LevelToCharacterStats);
			}
		}
	}
}

TMap<TSubclassOf<APRBaseCharacter>, FPRLevelToCharacterStat> UProjectReplicaGameInstance::GetCharacterStatSettings() const
{
	return CharacterStatSettings;
}
#pragma endregion 
