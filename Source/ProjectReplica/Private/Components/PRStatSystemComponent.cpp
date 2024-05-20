// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStatSystemComponent.h"

UPRStatSystemComponent::UPRStatSystemComponent()
{
	Level = 1;
	CharacterStat = FPRCharacterStat();
}

void UPRStatSystemComponent::InitializeStatSystem(int32 NewLevel, const FPRCharacterStat& NewCharacterStat)
{
	Level = NewLevel;
	SetCharacterStat(NewCharacterStat);
}

void UPRStatSystemComponent::SetHealth(float NewHealth)
{
	CharacterStat.Health = NewHealth;
}

void UPRStatSystemComponent::SetCharacterStat(const FPRCharacterStat& NewCharacterStat)
{
	CharacterStat = NewCharacterStat;
}
