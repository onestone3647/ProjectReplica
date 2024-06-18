// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "AnimNotifies/ANS_PRTimedNiagaraEffect.h"
#include "ANS_PRNiagaraEffectTrail.generated.h"

/**
 * 캐릭터의 EffectSystem에서 가져온 NiagaraEffect Trail을 가져와 Spawn하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRNiagaraEffectTrail : public UANS_PRTimedNiagaraEffect
{
	GENERATED_BODY()

public:
	UANS_PRNiagaraEffectTrail(const FObjectInitializer& ObjectInitializer);

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** Trail의 시작 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect")
	FName StartSocket;

	/** Trail의 끝 Socket의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EffectSystem|NiagaraEffect")
	FName EndSocket;
};
