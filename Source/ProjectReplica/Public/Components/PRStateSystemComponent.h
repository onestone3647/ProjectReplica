// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRStateSystemComponent.generated.h"

/**
 * 캐릭터의 상태(State)를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRStateSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStateSystemComponent();

public:
	/** 죽었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsDead() const;

	/** 무적 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsInvincible() const;

	/** 동작을 강제로 중단할 수 있는 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsInterruptible() const;

	/** 방어 상태인지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "StateSystem")
	bool IsBlocking() const;
	
private:
	/** 죽었는지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	/** 무적상태인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible;

	/** 동작을 강제로 중단할 수 있는 상태인지 나타내는 변수입니다.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	bool bIsInterruptible;

	/** 방어 상태인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CharacterState", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

public:
	/** 입력받은 인자로 IsDead를 설정하는 함수입니다. */
	void SetIsDead(bool bNewIsDead);

	/** 입력받은 인자로 bIsInvincible을 설정하는 함수입니다. */
	void SetIsInvincible(bool bNewIsInvincible);

	/** 입력받은 인자로 bIsInterruptible을 설정하는 함수입니다. */
	void SetIsInterruptible(bool bNewIsInterruptible);

	/** 입력받은 인자로 bIsBlocking을 설정하는 함수입니다. */
	void SetIsBlocking(bool bNewIsBlocking);
};
