// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRVault.h"
#include "Characters/PRPlayerCharacter.h"

void UANS_PRVault::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(MeshComp)
	{
		APRPlayerCharacter* PRPlayer = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayer))
		{
			PRPlayer->SetVaultState();
		}
	}
}

void UANS_PRVault::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if(MeshComp)
	{
		APRPlayerCharacter* PRPlayer = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayer))
		{
			PRPlayer->ResetVaultState();
		}
	}
}
