// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRNiagaraEffectTrail.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PREffectSystemComponent.h"
#include "NiagaraComponent.h"

UANS_PRNiagaraEffectTrail::UANS_PRNiagaraEffectTrail(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SocketName = TEXT("root");
	StartSocket = NAME_None;
	EndSocket =	NAME_None;
}

void UANS_PRNiagaraEffectTrail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if(MeshComp)
	{
		if(IsValid(NiagaraEffect))
		{
			NiagaraEffect->GetNiagaraEffect()->SetVectorParameter(TEXT("StartSocket"), MeshComp->GetSocketLocation(StartSocket));
			NiagaraEffect->GetNiagaraEffect()->SetVectorParameter(TEXT("EndSocket"), MeshComp->GetSocketLocation(EndSocket));
		}
		else
		{
#if WITH_EDITOR
			PR_LOG_SCREEN_INFO(0, "%s NiagaraEffect does not exist in the EffectSystem", *Template.GetName());
#endif
			
			UFXSystemComponent* FXComponent = GetSpawnedEffect(MeshComp);
			if(IsValid(FXComponent))
			{
				FXComponent->SetVectorParameter(TEXT("StartSocket"), MeshComp->GetSocketLocation(StartSocket));
				FXComponent->SetVectorParameter(TEXT("EndSocket"), MeshComp->GetSocketLocation(EndSocket));
			}
		}
	}
	
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}
