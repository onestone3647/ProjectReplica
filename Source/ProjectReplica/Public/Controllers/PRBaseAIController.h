// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "AIController.h"
#include "PRBaseAIController.generated.h"

/**
 * 기본 AI Controller 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API APRBaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	APRBaseAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

#pragma region AI
public:
	/** AI를 실행하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StartAI();

	/** AI를 정지하는 함수입니다. */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopAI();
	
protected:
	/** AI가 사용할 비헤이비어 트리입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	/** AI가 사용할 블랙보드입니다. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	UBlackboardData* BlackboardAsset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	FName AttackTargetKeyName;
#pragma endregion 
	
};
