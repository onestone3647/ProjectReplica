// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "AN_PRPlayNiagaraEffect.generated.h"

/**
 * 캐릭터의 EffectSystem에서 NiagaraEffect를 가져와 Spawn하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRPlayNiagaraEffect : public UAnimNotify_PlayNiagaraEffect
{
	GENERATED_BODY()

protected:
	/** NiagaraSystemComponent를 Spawn하는 함수입니다. Notify에서 호출됩니다. */
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
