// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState_TimedParticleEffect.h"
#include "ANS_PRTimedParticleEffect.generated.h"

/**
 * 캐릭터의 EffectSystem에서 ParticleEffect를 가져와 Spawn하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRTimedParticleEffect : public UAnimNotifyState_TimedParticleEffect
{
	GENERATED_BODY()

public:
	UANS_PRTimedParticleEffect(const FObjectInitializer& ObjectInitializer);

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** ParticleEffect를 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|ParticleEffect")
	APRParticleEffect* SpawnParticleEffect(USkeletalMeshComponent* MeshComp);

protected:
	/** Spawn한 ParticleEffect입니다. */
	UPROPERTY(BlueprintReadWrite, Category ="EffectSystem|ParticleEffect")
	TObjectPtr<APRParticleEffect> ParticleEffect;
	
};
