// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRDisableRootLock.h"
#include "AnimInstances/PRBaseAnimInstance.h"

void UAN_PRDisableRootLock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	DisableRootLock(MeshComp);
}

void UAN_PRDisableRootLock::DisableRootLock(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		if(UPRBaseAnimInstance* BaseAnimInstance = Cast<UPRBaseAnimInstance>(MeshComp->GetAnimInstance()))
		{
			BaseAnimInstance->DisableRootLock();
		}
	}
}
