// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "AnimNotifyState_TimedNiagaraEffect.h"
#include "ANS_PRTimedNiagaraEffect.generated.h"

class APRNiagaraEffect;

/**
 * 캐릭터의 EffectSystem에서 NiagaraEffect를 가져와 Spawn하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRTimedNiagaraEffect : public UAnimNotifyState_TimedNiagaraEffect
{
	GENERATED_BODY()

public:
	UANS_PRTimedNiagaraEffect(const FObjectInitializer& ObjectInitializer);

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** NiagaraEffect를 Spawn하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "EffectSystem|NiagaraEffect")
	APRNiagaraEffect* SpawnNiagaraEffect(USkeletalMeshComponent* MeshComp);

protected:
	/** Spawn한 NiagaraEffect입니다. */
	UPROPERTY(BlueprintReadWrite, Category ="EffectSystem|NiagaraEffect")
	TObjectPtr<APRNiagaraEffect> NiagaraEffect;
};
