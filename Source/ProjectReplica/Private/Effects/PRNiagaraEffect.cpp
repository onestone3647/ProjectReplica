// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/PRNiagaraEffect.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"

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
		NiagaraEffect->Deactivate();
	}
}

void APRNiagaraEffect::ActivateEffect(bool bReset)
{
	Super::ActivateEffect();

	if(IsValid(NiagaraEffect))
	{
		NiagaraEffect->Activate(bReset);
	}
}

void APRNiagaraEffect::DeactivateEffect()
{
	Super::DeactivateEffect();

	if(IsValid(NiagaraEffect))
	{
		NiagaraEffect->Deactivate();
	}
}

UFXSystemComponent* APRNiagaraEffect::GetFXSystemComponent() const
{
	return NiagaraEffect;	
}

UNiagaraSystem* APRNiagaraEffect::GetNiagaraEffectAsset() const
{
	if(IsValid(NiagaraEffect))
	{
		return NiagaraEffect->GetAsset();
	}

	return nullptr;
}

TObjectPtr<UNiagaraComponent> APRNiagaraEffect::GetNiagaraEffect() const
{
	return NiagaraEffect;
}
