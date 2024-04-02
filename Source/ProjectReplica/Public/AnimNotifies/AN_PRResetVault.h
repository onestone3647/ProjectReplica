// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRResetVault.generated.h"

/**
 * 캐릭터가 장애물을 뛰어넘은 후 상태를 초기화하는 AnimNotify 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRResetVault : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAN_PRResetVault(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
