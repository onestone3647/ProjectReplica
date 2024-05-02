// Fill out your copyright notice in the Description page of Project Settings.


#include "Effects/PRParticleEffect.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

APRParticleEffect::APRParticleEffect()
{
	ParticleEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEffect"));
	SetRootComponent(ParticleEffect);
}

void APRParticleEffect::InitializeParticleEffect(UParticleSystem* ParticleSystem, AActor* NewEffectOwner, int32 NewPoolIndex, float NewLifespan)
{
	InitializeEffect(NewEffectOwner, NewPoolIndex, NewLifespan);

	if(IsValid(ParticleEffect))
	{
		ParticleEffect->SetTemplate(ParticleSystem);
	}
}

void APRParticleEffect::Activate()
{
	Super::Activate();

	if(IsValid(ParticleEffect))
	{
		ParticleEffect->Activate();
	}
}

void APRParticleEffect::Deactivate()
{
	Super::Deactivate();

	if(IsValid(ParticleEffect))
	{
		ParticleEffect->Deactivate();
	}
}

UFXSystemComponent* APRParticleEffect::GetFXSystemComponent() const
{
	return ParticleEffect;
}

UParticleSystem* APRParticleEffect::GetParticleEffectAsset() const
{
	if(IsValid(ParticleEffect))
	{
		return Cast<UParticleSystem>(ParticleEffect->GetFXSystemAsset());
	}

	return nullptr;
}

TObjectPtr<UParticleSystemComponent> APRParticleEffect::GetParticleEffect() const
{
	return ParticleEffect;
}
