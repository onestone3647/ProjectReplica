// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify_PlayParticleEffect.h"
#include "AN_PRPlayParticleEffect.generated.h"

/**
 * 캐릭터의 EffectSystem에서 ParticleEffect를 가져와 Spawn하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRPlayParticleEffect : public UAnimNotify_PlayParticleEffect
{
	GENERATED_BODY()

protected:
	/** ParticleSystemComponent를 Spawn하는 함수입니다. Notify에서 호출됩니다. */
	virtual UParticleSystemComponent* SpawnParticleSystem(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
