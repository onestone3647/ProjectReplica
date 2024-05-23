 // Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PRBaseAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/PRBaseCharacter.h"

APRBaseAIController::APRBaseAIController()
{
	BehaviorTreeAsset = nullptr;
	BlackboardAsset = nullptr;
	AttackTargetKeyName = TEXT("AttackTarget");
}

void APRBaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 딜레이를 줘야 BehaviorTree가 정상적으로 실행됨
	float StartAIDelay = 0.2f;
	FTimerHandle StartAIDelayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StartAIDelayTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		StartAI();
	}), StartAIDelay, false);
}

void APRBaseAIController::StartAI()
{
	// AI가 BlackboardAsset을 사용합니다.
	UBlackboardComponent* LocalBlackboardComponent = Blackboard.Get();
	if(UseBlackboard(BlackboardAsset, LocalBlackboardComponent))
	{
		// AI가 BehaviorTreeAsset을 사용합니다.
		if(RunBehaviorTree(BehaviorTreeAsset))
		{
			// Blackboard의 Key 값을 설정합니다.
			GetBlackboardComponent()->SetValueAsObject(AttackTargetKeyName, UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		}
		else
		{
			PR_LOG_ERROR("AIController couldn't run BehaviorTree");
		}
	}
}

void APRBaseAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if(BehaviorTreeComponent != nullptr)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
}
