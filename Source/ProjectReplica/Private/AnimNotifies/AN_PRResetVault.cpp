// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRResetVault.h"
#include "Characters/PRPlayerCharacter.h"

UAN_PRResetVault::UAN_PRResetVault(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

void UAN_PRResetVault::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(MeshComp)
	{
		APRPlayerCharacter* PRPlayer = Cast<APRPlayerCharacter>(MeshComp->GetOwner());
		if(IsValid(PRPlayer))
		{
			// PRPlayer->OnVaultAnimMontageEnded(nullptr, false);
		}
	}
}
