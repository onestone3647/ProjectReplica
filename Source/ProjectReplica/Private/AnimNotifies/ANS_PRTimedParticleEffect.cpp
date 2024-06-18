// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRTimedParticleEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"

UANS_PRTimedParticleEffect::UANS_PRTimedParticleEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ParticleEffect = nullptr;
}

void UANS_PRTimedParticleEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	ParticleEffect = SpawnParticleEffect(MeshComp);
	if(IsValid(ParticleEffect))
	{
		UAnimNotifyState::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	}
	else
	{
		Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	}
}

void UANS_PRTimedParticleEffect::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if(IsValid(ParticleEffect))
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner))
		{
			UPREffectSystemComponent* EffectSystem = PROwner->GetEffectSystem();
			if(EffectSystem)
			{
				EffectSystem->DeactivateObject(ParticleEffect);
			}
		}
	}
	else
	{
		Super::NotifyEnd(MeshComp, Animation, EventReference);
	}
}

APRParticleEffect* UANS_PRTimedParticleEffect::SpawnParticleEffect(USkeletalMeshComponent* MeshComp)
{
	if (ValidateParameters(MeshComp))
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner))
		{
			UPREffectSystemComponent* EffectSystem = PROwner->GetEffectSystem();
			if(EffectSystem)
			{
				return EffectSystem->SpawnParticleEffectAttached(PSTemplate, MeshComp, SocketName, LocationOffset, RotationOffset);
			}
		}
	}
	
	return nullptr;
}
