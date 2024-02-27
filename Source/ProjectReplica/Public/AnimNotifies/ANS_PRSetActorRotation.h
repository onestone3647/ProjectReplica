// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PRSetActorRotation.generated.h"

/**
 * NotifyBegin에서 현재 액터의 회전 값을 저장하고 NotifyEnd에서 설정한 회전 값을 더하여 액터의 회전 값을 설정하는 AnimNotifyState 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UANS_PRSetActorRotation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UANS_PRSetActorRotation(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** 입력받은 인자의 Owner의 현재 회전 값을 CurrentRotation에 저장하는 함수입니다. */
	void SetCurrentRotation(USkeletalMeshComponent* MeshComp);
	
	/** 입력받은 인자의 Owner의 RootLock을 설정하는 함수입니다. */
	void SetOwnerRotation(USkeletalMeshComponent* MeshComp);

protected:
	/** 현재 회전 값입니다. */
	UPROPERTY(BlueprintReadOnly, Category = "PRSetActorRotation")
	FRotator CurrentRotation;
	
	/** 적용할 회전 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRSetActorRotation")
	FRotator Rotation;
};
