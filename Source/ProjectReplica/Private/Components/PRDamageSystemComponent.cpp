// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRDamageSystemComponent.h"
#include "ProjectReplicaGameInstance.h"
#include "ProjectReplicaGameMode.h"
#include "Components/PRStatSystemComponent.h"
#include "Components/PRStateSystemComponent.h"
#include "Components/PRObjectPoolSystemComponent.h"
#include "Characters/PRBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Objects/PRDamageAmount.h"

UPRDamageSystemComponent::UPRDamageSystemComponent()
{
	// InitializeComponent 함수를 실행하기 위해서 true로 설정합니다.
	bWantsInitializeComponent = true;

	StatSystem = nullptr;
	StateSystem = nullptr;
}

void UPRDamageSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(GetPROwner())
	{
		// StatSystem을 바인딩합니다.
		BindStatSystem(GetPROwner()->GetStatSystem());

		// StateSystem을 바인딩합니다.
		BindStateSystem(GetPROwner()->GetStateSystem());
	}
}

void UPRDamageSystemComponent::BindStatSystem(TObjectPtr<UPRStatSystemComponent> NewStatSystem)
{
	StatSystem = NewStatSystem;
}

void UPRDamageSystemComponent::BindStateSystem(TObjectPtr<UPRStateSystemComponent> NewStateSystem)
{
	StateSystem = NewStateSystem;
}

float UPRDamageSystemComponent::Heal(float Amount)
{
	if(GetPROwner()
		&& !GetPROwner()->IsDead()
		&& StatSystem.IsValid())
	{
		FPRCharacterStat CharacterStat = StatSystem->GetCharacterStat();
		StatSystem->SetHealth(FMath::Clamp(CharacterStat.Health + Amount, 0.0f, CharacterStat.MaxHealth));
		
		return CharacterStat.Health;
	}

	return 0.0f;
}

bool UPRDamageSystemComponent::TakeDamage(FPRDamageInfo DamageInfo)
{
	if(!GetPROwner()
		|| GetPROwner()->IsDead()
		|| !StatSystem.IsValid()
		|| !StateSystem.IsValid())
	{
		return false;
	}

	FPRCharacterStat CharacterStat = StatSystem->GetCharacterStat();

	// 대미지의 처리를 구분하여 실행합니다.
	EPRCanBeDamaged CanBeDamagedResult = CanBeDamaged(DamageInfo.bShouldDamageInvincible, DamageInfo.bCanBeBlocked);
	switch(CanBeDamagedResult)
	{
	case EPRCanBeDamaged::CanBeDamaged_BlockDamage:
		if(OnBlockedDelegate.IsBound())
		{
			// 방어 상태이면서 방어할 수 있는 대미지이므로 패링합니다.
			OnBlockedDelegate.Broadcast(DamageInfo.bCanBeParried);
		}
		break;
		
	case EPRCanBeDamaged::CanBeDamaged_DoDamage:
		{
			if(GetPRGameMode() != nullptr)
			{
				GetPRGameMode()->ActivateDamageAmount(DamageInfo.ImpactLocation, DamageInfo.Amount, DamageInfo.bIsCritical, DamageInfo.DamageElement);
			}
		
			StatSystem->SetHealth(CharacterStat.Health -= DamageInfo.Amount);
			if(CharacterStat.Health <= 0.0f)
			{
				// 캐릭터의 체력이 0이하(사망)일 경우 OnDeathDelegate를 실행합니다.
				if(OnDeathDelegate.IsBound())
				{
					OnDeathDelegate.Broadcast();
				}
			}
			else
			{
				// 동작을 강제로 중단할 수 있는 상태이거나 동작을 강제로 중단해야하는 대미지일 경우
				// OnDamageResponseDelegate를 실행합니다.
				if(StateSystem->IsInterruptible() || DamageInfo.bShouldForceInterrupt)
				{
					if(OnDamageResponseDelegate.IsBound())
					{
						OnDamageResponseDelegate.Broadcast(DamageInfo.DamageResponse);
						return true;
					}
				}
			}
		}
		break;
		
	case EPRCanBeDamaged::CanBeDamaged_NoDamage:
	default:
		break;
	}
	
	return false;
}

EPRCanBeDamaged UPRDamageSystemComponent::CanBeDamaged(const bool& bShouldDamageInvincible, const bool& bCanBeBlocked)
{
	if(GetPROwner()
		&& !GetPROwner()->IsDead()
		&& (!GetPROwner()->IsInvincible() || bShouldDamageInvincible))
	{
		if(GetPROwner()->IsBlocking() && bCanBeBlocked)
		{
			return EPRCanBeDamaged::CanBeDamaged_BlockDamage;
		}
		
		return EPRCanBeDamaged::CanBeDamaged_DoDamage;
	}

	return EPRCanBeDamaged::CanBeDamaged_NoDamage;
}
