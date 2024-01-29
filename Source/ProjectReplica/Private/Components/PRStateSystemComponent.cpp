// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRStateSystemComponent.h"

UPRStateSystemComponent::UPRStateSystemComponent()
{
 // CharacterState
 bIsDead = false;
 bIsInvincible = false;
 bIsInterruptible = true;
 bIsBlocking = false;
}

bool UPRStateSystemComponent::IsDead() const
{
 return bIsDead;
}

bool UPRStateSystemComponent::IsInvincible() const
{
 return bIsInvincible;
}

bool UPRStateSystemComponent::IsInterruptible() const
{
 return bIsInterruptible;
}

bool UPRStateSystemComponent::IsBlocking() const
{
 return bIsBlocking;
}

void UPRStateSystemComponent::SetIsDead(bool bNewIsDead)
{
 bIsDead = bNewIsDead;
}

void UPRStateSystemComponent::SetIsInvincible(bool bNewIsInvincible)
{
 bIsInvincible = bNewIsInvincible;
}

void UPRStateSystemComponent::SetIsInterruptible(bool bNewIsInterruptible)
{
 bIsInterruptible = bNewIsInterruptible;
}

void UPRStateSystemComponent::SetIsBlocking(bool bNewIsBlocking)
{
 bIsBlocking = bNewIsBlocking;
}
