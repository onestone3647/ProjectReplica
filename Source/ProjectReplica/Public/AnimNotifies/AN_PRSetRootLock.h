// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_PRSetRootLock.generated.h"

/**
 * 캐릭터의 RootLock을 설정하여 재생하는 애니메이션의 RootMotion을 실행할지 설정하는 AnimNotify 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UAN_PRSetRootLock : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAN_PRSetRootLock(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override;

protected:
	/** 입력받은 인자의 Owner의 RootLock을 설정하는 함수입니다. */
	void SetRootLock(USkeletalMeshComponent* MeshComp);

protected:
	/** RootLock 실행 여부입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SetLastFootOnLand")
	bool bRootLock;
};
