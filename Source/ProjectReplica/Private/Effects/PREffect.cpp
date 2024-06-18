// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/PREffect.h"
#include "Particles/ParticleSystemComponent.h"

APREffect::APREffect()
{
	PrimaryActorTick.bCanEverTick = false;

	bActivate = false;
	EffectLifespan = 0.0f;
	EffectOwner = nullptr;
	PoolIndex = INDEX_NONE;
}

void APREffect::BeginPlay()
{
	Super::BeginPlay();
	
}

#pragma region PooledableInterface
bool APREffect::IsActivate_Implementation() const
{
	return bActivate;
}

void APREffect::Activate_Implementation()
{
	ActivateEffect();
}

void APREffect::Deactivate_Implementation()
{
	DeactivateEffect();
}

int32 APREffect::GetPoolIndex_Implementation() const
{
	return PoolIndex;
}
#pragma endregion 

void APREffect::SpawnEffectAtLocation(FVector Location, FRotator Rotation, FVector Scale, bool bAutoActivate, bool bReset)
{
	SetActorLocationAndRotation(Location, Rotation);
	SetActorScale3D(Scale);
	if(bAutoActivate)
	{
		ActivateEffect(bReset);
	}
}

void APREffect::SpawnEffectAttached(USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoActivate, bool bReset)
{
	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
	AttachToComponent(Parent, AttachmentTransformRules, AttachSocketName);
	if (LocationType == EAttachLocation::KeepWorldPosition)
	{
		SetActorLocationAndRotation(Parent->GetSocketLocation(AttachSocketName) + Location, Parent->GetSocketRotation(AttachSocketName) + Rotation);
	}
	else
	{
		SetActorRelativeLocation(Location);
		SetActorRelativeRotation(Rotation);
	}
	SetActorScale3D(Scale);
	if(bAutoActivate)
	{
		ActivateEffect(bReset);
	}
}

void APREffect::ActivateEffect(bool bReset)
{
	bActivate = true;
	SetActorHiddenInGame(!bActivate);

	// 이펙트의 수명을 설정합니다. 이펙트의 수명이 끝나면 이펙트를 비활성화합니다.
	SetEffectLifespan(EffectLifespan);
}

void APREffect::DeactivateEffect()
{
	bActivate = false;
	SetActorHiddenInGame(!bActivate);

	// 이펙트에 설정된 모든 타이머를 초기화합니다.
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// 비활성화 델리게이트를 호출합니다.
	OnEffectDeactivateDelegate.Broadcast(this);
}

UFXSystemComponent* APREffect::GetFXSystemComponent() const
{
	// 자식 클래스에서 오버라이딩합니다.
	
	return nullptr;
}

void APREffect::InitializeEffect(AActor* NewEffectOwner, int32 NewPoolIndex, float NewLifespan)
{
	// 이펙트를 비활성화 상태로 설정합니다.
	bActivate = false;
	SetActorHiddenInGame(!bActivate);

	EffectOwner = NewEffectOwner;
	PoolIndex = NewPoolIndex;
	EffectLifespan = NewLifespan;

	// 이펙트에 설정된 모든 타이머를 초기화합니다.
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// 비활성화 델리게이트에 바인딩된 함수를 제거합니다.
	OnEffectDeactivateDelegate.Clear();
}

void APREffect::SetEffectLifespan(float NewLifespan)
{
	EffectLifespan = NewLifespan;
	if(bActivate)
	{
		if(NewLifespan > 0.0f)
		{
			// 수명이 0보다 클 경우, 즉 새로운 수명이 설정된 경우 타이머를 설정합니다.
			GetWorldTimerManager().SetTimer(EffectLifespanTimerHandle, this, &APREffect::OnDeactivate, NewLifespan);
		}
		else
		{
			// 수명이 0보다 작거나 같을 경우, 즉 이펙트의 수명이 무한대인 경우 이전에 설정한 타이머를 지워 제한된 수명을 가지지 않게 합니다.
			GetWorldTimerManager().ClearTimer(EffectLifespanTimerHandle);
		}
	}
}

void APREffect::OnDeactivate()
{
	IPRPoolableInterface::Execute_Deactivate(this);
}

float APREffect::GetEffectLifespan() const
{
	return EffectLifespan;
}

