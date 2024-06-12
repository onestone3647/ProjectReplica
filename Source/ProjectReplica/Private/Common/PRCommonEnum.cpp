// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/PRCommonEnum.h"

FString PRCommonEnum::GetEnumDisplayNameToString(const TCHAR* Enum, int32 EnumValue)
{
	// const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);		// Unreal Engine 5 이전 버전, Unreal Engine 5에서는 ANY_PACKAGE는 더이상 사용되지 않습니다.
	const UEnum* EnumPtr = FindFirstObjectSafe<UEnum>(Enum);
	if(EnumPtr == nullptr)
	{
		return FString("Invalid");
	}

	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
}
