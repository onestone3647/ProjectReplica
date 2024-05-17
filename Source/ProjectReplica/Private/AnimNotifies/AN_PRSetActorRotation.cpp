// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetActorRotation.h"

UAN_PRSetActorRotation::UAN_PRSetActorRotation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Rotation = FRotator::ZeroRotator;
}

void UAN_PRSetActorRotation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	SetOwnerRotation(MeshComp);
}

void UAN_PRSetActorRotation::SetOwnerRotation(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp && MeshComp->GetOwner())
	{
		FRotator CurrentRotation = MeshComp->GetOwner()->GetActorRotation();
		MeshComp->GetOwner()->SetActorRotation(CurrentRotation + Rotation);
	}
}
