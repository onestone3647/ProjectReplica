// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/PlayerController.h"
#include "PRPlayerController.generated.h"

/**
 * 플레이어가 사용하는 PlayerController 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API APRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APRPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	/** 게임패드를 사용한 입력인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "InputDevice")
	bool IsUsingGamepad() const;
};
