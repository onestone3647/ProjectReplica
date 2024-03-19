// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRPlayFootsteps.generated.h"

/**
 * 캐릭터의 발소리를 재생하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRPlayFootsteps : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRPlayFootsteps(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	/** 발소리를 재생하는 함수입니다. */
	void PlayFootsteps(USkeletalMeshComponent* MeshComp);

private:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebug;

	/** 발바닥에서부터 발소리를 출력할 피직스 머테리얼을 탐색하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps", meta = (AllowPrivateAccess = "true"))
	float TraceDistance;
	
};
