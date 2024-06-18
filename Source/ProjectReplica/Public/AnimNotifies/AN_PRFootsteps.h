// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRFootsteps.generated.h"

/**
 * 캐릭터의 발소리를 재생하고 발걸음 이펙트를 실행하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRFootsteps : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAN_PRFootsteps(const FObjectInitializer& ObjectInitializer);

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	/** 발소리를 재생하는 함수입니다. */
	void PlayFootsteps(USkeletalMeshComponent* MeshComp);

	/** 발걸음 이펙트를 실행하는 함수입니다. */
	void ActivateFootstepsEffect(USkeletalMeshComponent* MeshComp);

private:
	/** 디버그 실행을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebug;

	/** 발바닥에서부터 발소리를 출력할 피직스 머테리얼을 탐색하는 거리입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps", meta = (AllowPrivateAccess = "true"))
	float TraceDistance;

	/** 발걸음 이펙트의 위치 오프셋입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Effect", meta = (AllowPrivateAccess = "true"))
	FVector EffectLocationOffset;

	/** 발걸음 이펙트의 회전 오프셋입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Effect", meta = (AllowPrivateAccess = "true"))
	FRotator EffectRotationOffset;

	/** 발걸음 이펙트의 크기 오프셋입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Effect", meta = (AllowPrivateAccess = "true"))
	FVector EffectScaleOffset;

	/** 발걸음 이펙트를 Spawn할 위치에 해당하는 본의 이름입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PRPlayFootsteps|Effect", meta = (AllowPrivateAccess = "true"))
	FName BoneName;	
};
