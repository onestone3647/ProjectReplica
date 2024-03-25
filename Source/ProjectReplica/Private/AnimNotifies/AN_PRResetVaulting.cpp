// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRResetVaulting.h"
#include "Characters/PRPlayerCharacter.h"

void UAN_PRResetVaulting::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ResetVaulting(MeshComp);
}

void UAN_PRResetVaulting::ResetVaulting(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		APRPlayerCharacter* PRPlayer = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayer))
		{
			PRPlayer->ResetVaulting();
		}
	}
}
