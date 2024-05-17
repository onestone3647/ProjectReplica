// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRDisableRootLock.generated.h"

/**
 * 캐릭터의 RootLock을 비활성화하여 Montage만 RootMotion을 실행하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRDisableRootLock : public UAnimNotify
{
	GENERATED_BODY()
	
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** 입력받은 인자의 Owner의 RootLock을 비활성화하는 함수입니다. */
	void DisableRootLock(USkeletalMeshComponent* MeshComp);
};
