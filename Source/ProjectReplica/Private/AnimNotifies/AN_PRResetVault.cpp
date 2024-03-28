// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRResetVault.h"
#include "Characters/PRPlayerCharacter.h"

void UAN_PRResetVault::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	ResetVault(MeshComp);
}

void UAN_PRResetVault::ResetVault(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		APRPlayerCharacter* PRPlayer = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayer))
		{
			PRPlayer->ResetVault();
		}
	}
}
