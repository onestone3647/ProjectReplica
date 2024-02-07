// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AN_PRSetLastFootOnLand.h"
#include "AnimInstances/PRBaseAnimInstance.h"

UAN_PRSetLastFootOnLand::UAN_PRSetLastFootOnLand(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LastFootOnLand = EPRFoot::EPRFoot_Left;
}

void UAN_PRSetLastFootOnLand::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	SetLastFootOnLand(MeshComp);
	
	Super::Notify(MeshComp, Animation, EventReference);
}

FString UAN_PRSetLastFootOnLand::GetNotifyName_Implementation() const
{
	FString NewNotifyName;
	NewNotifyName.Append(PRCommonEnum::GetEnumDisplayNameToString(TEXT("EPRFoot"), static_cast<uint8>(LastFootOnLand)));

	return NewNotifyName;
}

void UAN_PRSetLastFootOnLand::SetLastFootOnLand(USkeletalMeshComponent* MeshComp)
{
	if(MeshComp)
	{
		if(UPRBaseAnimInstance* BaseAnimInstance = Cast<UPRBaseAnimInstance>(MeshComp->GetAnimInstance()))
		{
			BaseAnimInstance->SetLastFootOnLand(LastFootOnLand);
		}		
	}
}

