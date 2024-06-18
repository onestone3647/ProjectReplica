// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRTimedNiagaraEffect.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"

UANS_PRTimedNiagaraEffect::UANS_PRTimedNiagaraEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NiagaraEffect = nullptr;
}

void UANS_PRTimedNiagaraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	NiagaraEffect = SpawnNiagaraEffect(MeshComp);
	if(IsValid(NiagaraEffect))
	{
		UAnimNotifyState::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	}
	else
	{
		Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	}
}

void UANS_PRTimedNiagaraEffect::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if(IsValid(NiagaraEffect))
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner))
		{
			UPREffectSystemComponent* EffectSystem = PROwner->GetEffectSystem();
			if(EffectSystem)
			{
				EffectSystem->DeactivateObject(NiagaraEffect);
			}
		}
	}
	else
	{
		Super::NotifyEnd(MeshComp, Animation, EventReference);
	}
}

APRNiagaraEffect* UANS_PRTimedNiagaraEffect::SpawnNiagaraEffect(USkeletalMeshComponent* MeshComp)
{
	if (ValidateParameters(MeshComp))
	{
		APRBaseCharacter* PROwner = Cast<APRBaseCharacter>(MeshComp->GetOwner());
		if(IsValid(PROwner))
		{
			UPREffectSystemComponent* EffectSystem = PROwner->GetEffectSystem();
			if(EffectSystem)
			{
				return EffectSystem->SpawnNiagaraEffectAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset);
			}
		}
	}
	
	return nullptr;
}
