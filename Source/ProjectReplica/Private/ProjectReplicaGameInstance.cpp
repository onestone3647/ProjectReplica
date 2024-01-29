// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectReplicaGameInstance.h"

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
}

FLinearColor UProjectReplicaGameInstance::GetElementColor(EPRElement Element) const
{
	if(ElementColors.Contains(Element))
	{
		return *ElementColors.Find(Element);
	}

	// 존재하지 않는 속성일 경우 물리 속성의 색상을 반환합니다.
	return *ElementColors.Find(EPRElement::Element_Physio);
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
				ElementColors.Emplace(DataTableRow->Element, DataTableRow->Color);
			}
		}
	}
}
