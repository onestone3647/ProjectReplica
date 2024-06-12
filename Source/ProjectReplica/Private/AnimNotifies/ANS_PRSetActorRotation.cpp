// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/ANS_PRSetActorRotation.h"

UANS_PRSetActorRotation::UANS_PRSetActorRotation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentRotation = FRotator::ZeroRotator;
	Rotation = FRotator::ZeroRotator;
}

void UANS_PRSetActorRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SetCurrentRotation(MeshComp);
}

void UANS_PRSetActorRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	SetOwnerRotation(MeshComp);
}

void UANS_PRSetActorRotation::SetCurrentRotation(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp && MeshComp->GetOwner())
	{
		CurrentRotation = MeshComp->GetOwner()->GetActorRotation();
		PR_LOG_SCREEN("Before %s", *CurrentRotation.ToString());
	}
}

void UANS_PRSetActorRotation::SetOwnerRotation(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp && MeshComp->GetOwner())
	{
		PR_LOG_SCREEN("like %s", *(CurrentRotation + Rotation).ToString());

		MeshComp->GetOwner()->SetActorRotation(CurrentRotation + Rotation, ETeleportType::TeleportPhysics);
		PR_LOG_SCREEN("After %s", *MeshComp->GetOwner()->GetActorRotation().ToString());
	}
}
