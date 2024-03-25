// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRResetVaulting.generated.h"

/**
 * Owner의 Vaulting을 초기화하는 함수(ResetVaulting)를 호출하는 AnimNotify 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRResetVaulting : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	/** Owner의 Vaulting을 초기화하는 함수를 호출하는 함수입니다. */
	void ResetVaulting(USkeletalMeshComponent* MeshComp);
};
