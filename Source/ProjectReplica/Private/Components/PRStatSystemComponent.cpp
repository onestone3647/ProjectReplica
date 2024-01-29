// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStatSystemComponent.h"

UPRStatSystemComponent::UPRStatSystemComponent()
{
	CharacterStat = FPRCharacterStat();
}

void UPRStatSystemComponent::SetHealth(float NewHealth)
{
	CharacterStat.Health = NewHealth;
}

void UPRStatSystemComponent::SetCharacterStat(FPRCharacterStat NewCharacterStat)
{
	CharacterStat = NewCharacterStat;
}
