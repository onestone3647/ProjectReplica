// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetActorRotation.generated.h"

/**
 * 액터의 회전 값을 적용하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetActorRotation : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetActorRotation(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	/** 입력받은 인자의 Owner의 RootLock을 설정하는 함수입니다. */
	void SetOwnerRotation(USkeletalMeshComponent* MeshComp);

protected:
	/** 적용할 회전 값입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PRSetActorRotation")
	FRotator Rotation;
};
