// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/PRNiagaraEffect.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

APRNiagaraEffect::APRNiagaraEffect()
{
	NiagaraEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	SetRootComponent(NiagaraEffect);
}

void APRNiagaraEffect::InitializeNiagaraEffect(UNiagaraSystem* NiagaraSystem, AActor* NewEffectOwner, int32 NewPoolIndex, float NewLifespan)
{
	InitializeEffect(NewEffectOwner, NewPoolIndex, NewLifespan);

	if(IsValid(NiagaraSystem))
	{
		NiagaraEffect->SetAsset(NiagaraSystem);
	}
}

void APRNiagaraEffect::Activate()
{
	Super::Activate();
	
	NiagaraEffect->Activate();
}

void APRNiagaraEffect::Deactivate()
{
	Super::Deactivate();

	NiagaraEffect->Deactivate();

	// 비활성화 델리게이트를 호출합니다.
	// OnNiagaraEffectDeactivate.Broadcast(this);
}

UNiagaraSystem* APRNiagaraEffect::GetNiagaraEffectAsset() const
{
	return NiagaraEffect->GetAsset();
}

TObjectPtr<UNiagaraComponent> APRNiagaraEffect::GetNiagaraEffect() const
{
	return NiagaraEffect;
}
