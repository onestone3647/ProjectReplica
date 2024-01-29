// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Engine/DataAsset.h"
#include "PRInputConfigDataAsset.generated.h"

class UInputAction;

/**
 * InputAction을 보관한 DataAsset 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 이동 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputMove;

	/** 마우스의 좌우 시선 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputTurn;

	/** 게임패드의 좌우 시선 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputTurnRate;

	/** 마우스의 상하 시선 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputLookUp;

	/** 게임패드의 상하 시선 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputLookUpRate;

	/** 점프 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputJump;

	/** 일반 공격 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputNormalAttack;	

	/** 걷기 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputWalk;

	/** 전력 질주 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputSprint;

	/** 상호 작용 입력입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UInputAction* InputInteraction;
};
