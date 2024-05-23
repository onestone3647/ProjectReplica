// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRWeaponSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "ProjectReplicaGameInstance.h"

UPRWeaponSystemComponent::UPRWeaponSystemComponent()
{
	EquippedWeaponClass = nullptr;
	WeaponState = EPRWeaponState::WeaponState_Sheathe;
	EquippedWeapon = nullptr;
}

void UPRWeaponSystemComponent::InitializeWeaponSystem()
{
	if(EquippedWeaponClass != nullptr)
	{
		EquipWeapon(EquippedWeaponClass);
	}
}

void UPRWeaponSystemComponent::DrawWeapon(bool bSpawnActivateEffect)
{
	if(IsValid(GetPROwner()) && IsValid(EquippedWeapon) && IsSheatheWeapon())
	{
		WeaponState = EPRWeaponState::WeaponState_Draw;
		EquippedWeapon->Draw(bSpawnActivateEffect);
	}
}

void UPRWeaponSystemComponent::SheatheWeapon(bool bSpawnActivateEffect, bool bVisible)
{
	if(IsValid(GetPROwner()) && IsValid(EquippedWeapon) && IsDrawWeapon())
	{
		WeaponState = EPRWeaponState::WeaponState_Sheathe;
		EquippedWeapon->Sheathe(bSpawnActivateEffect, bVisible);
	}
}

APRBaseWeapon* UPRWeaponSystemComponent::EquipWeapon(TSubclassOf<APRBaseWeapon> NewWeaponClass)
{
	APRBaseWeapon* NewEquipWeapon = nullptr;
	
	if(IsValid(GetPROwner()) && NewWeaponClass != nullptr)
	{
		const FPRWeaponStat WeaponStat = GetPRGameInstance()->GetWeaponStat(NewWeaponClass);
		// WeaponStat이 존재할 경우 무기를 월드에 Spawn하고 초기화한 후 반환합니다.
		if(WeaponStat != FPRWeaponStat())
		{
			NewEquipWeapon = SpawnWeaponInWorld(NewWeaponClass);
			if(IsValid(NewEquipWeapon))
			{
				// 현재 장착한 무기를 장착 해제합니다.
				UnequipWeapon();
				
				NewEquipWeapon->InitializeWeapon(GetPROwner(), WeaponStat);
				EquippedWeapon = NewEquipWeapon;
				SheatheWeapon(false);
			}
		}
	}

	return NewEquipWeapon;
}

void UPRWeaponSystemComponent::UnequipWeapon()
{
	if(IsValid(EquippedWeapon))
	{
		// 기존에 장착한 무기는 비활성화하고 월드에서 제거합니다.
		SheatheWeapon(false);
		EquippedWeapon->Deactivate();
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}
}

bool UPRWeaponSystemComponent::IsDrawWeapon() const
{
	return WeaponState == EPRWeaponState::WeaponState_Draw;
}

bool UPRWeaponSystemComponent::IsSheatheWeapon() const
{
	return WeaponState == EPRWeaponState::WeaponState_Sheathe;
}

APRBaseWeapon* UPRWeaponSystemComponent::SpawnWeaponInWorld(TSubclassOf<APRBaseWeapon> SpawnWeaponClass)
{
	return GetWorld()->SpawnActor<APRBaseWeapon>(SpawnWeaponClass);
}

APRBaseWeapon* UPRWeaponSystemComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}
