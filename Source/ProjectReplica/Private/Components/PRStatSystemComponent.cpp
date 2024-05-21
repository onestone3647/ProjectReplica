// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStatSystemComponent.h"
#include "ProjectReplicaGameInstance.h"
#include "Characters/PRBaseCharacter.h"

UPRStatSystemComponent::UPRStatSystemComponent()
{
	Level = 1;
	CharacterStat = FPRCharacterStat();
}

void UPRStatSystemComponent::InitializeStatByLevel(int32 NewLevel)
{
	if(GetPROwner() && GetPRGameInstance())
	{
		Level = NewLevel;
		
		const FPRCharacterStat NewCharacterStat = GetPRGameInstance()->GetCharacterStat(GetPROwner()->GetClass(), NewLevel);
		SetCharacterStat(NewCharacterStat);
	}
}

void UPRStatSystemComponent::SetHealth(float NewHealth)
{
	CharacterStat.Health = NewHealth;
}

FPRCharacterStat UPRStatSystemComponent::GetCharacterStat() const
{
	return CharacterStat;
}

void UPRStatSystemComponent::SetCharacterStat(const FPRCharacterStat& NewCharacterStat)
{
	CharacterStat = NewCharacterStat;
}
