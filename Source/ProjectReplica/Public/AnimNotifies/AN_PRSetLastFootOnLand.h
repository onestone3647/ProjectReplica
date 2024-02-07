// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetLastFootOnLand.generated.h"

enum class EPRFoot : uint8;

/**
 * 캐릭터의 마지막으로 바닥에 닿은 발을 설정하는 AnimNotify 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetLastFootOnLand : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetLastFootOnLand(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** 입력받은 인자의 Owner의 마지막으로 바닥에 닿은 발의 위치를 설정하는 함수입니다. */
	void SetLastFootOnLand(USkeletalMeshComponent* MeshComp);

protected:
	/** 마지막으로 바닥에 닿은 발의 위치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetLastFootOnLand")
	EPRFoot LastFootOnLand;
};
