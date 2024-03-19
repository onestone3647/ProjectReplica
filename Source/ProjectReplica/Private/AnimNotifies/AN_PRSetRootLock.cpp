// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetRootLock.h"
#include "AnimInstances/PRBaseAnimInstance.h"

UAN_PRSetRootLock::UAN_PRSetRootLock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bRootLock = false;
}

void UAN_PRSetRootLock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	SetRootLock(MeshComp);
}

FString UAN_PRSetRootLock::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	if(bRootLock)
	{
		// RootLock 활성화
		NewNotifyName.Append("Enable RootLock");
	}
	else
	{
		// RootLock 비활성화
		NewNotifyName.Append("Disable RootLock");
	}
	
	return NewNotifyName;
}

void UAN_PRSetRootLock::SetRootLock(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		if(UPRBaseAnimInstance* BaseAnimInstance = Cast<UPRBaseAnimInstance>(MeshComp->GetAnimInstance()))
		{
			BaseAnimInstance->SetRootLock(bRootLock);
		}
	}
}
