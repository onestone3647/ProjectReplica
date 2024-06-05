// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRBaseObjectPoolSystemComponent.h"

UPRBaseObjectPoolSystemComponent::UPRBaseObjectPoolSystemComponent()
{
	DynamicDestroyDelay = 60.0f;
	DynamicPoolSize = 3;
}

void UPRBaseObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	// 모든 ObjectPool을 제거합니다.
	ClearAllObjectPool();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPRBaseObjectPoolSystemComponent::InitializeObjectPool()
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.
}

void UPRBaseObjectPoolSystemComponent::ClearAllObjectPool()
{
	// 자식 클래스에서 오버라이딩하여 사용합니다.
}

void UPRBaseObjectPoolSystemComponent::ClearDynamicDestroyObjectList(FPRDynamicDestroyObjectList& NewDynamicDestroyObjectList)
{
	// DynamicDestroyObjectList의 모든 클래스에 대해 반복합니다.
	for(auto& ListEntry : NewDynamicDestroyObjectList.List)
	{
		FPRDynamicDestroyObject& DynamicDestroyObject = ListEntry.Value;
		
		// DynamicDestroyObject의 모든 타이머를 해제하고 오브젝트를 제거합니다.
		for(auto& TimerEntry : DynamicDestroyObject.TimerHandles)
		{
			if(IsValid(TimerEntry.Key))
			{
				// 타이머를 해제합니다.
				GetWorld()->GetTimerManager().ClearTimer(TimerEntry.Value);

				// 오브젝트를 제거합니다.
				TimerEntry.Key->ConditionalBeginDestroy();		// 오브젝트를 안전하게 제거하는 함수입니다. 가비지 컬렉션 대상이 되기 전에 수동으로 메모리에서 해제합니다.
				TimerEntry.Key = nullptr;
			}
		}

		DynamicDestroyObject.TimerHandles.Empty();
	}

	NewDynamicDestroyObjectList.List.Empty();
}

bool UPRBaseObjectPoolSystemComponent::IsPoolableObject(UObject* PoolableObject) const
{
	return IsValid(PoolableObject) && PoolableObject->GetClass()->ImplementsInterface(UPRPoolableInterface::StaticClass());
}

bool UPRBaseObjectPoolSystemComponent::IsPoolableObjectClass(TSubclassOf<UObject> PoolableObjectClass) const
{
	return IsValid(PoolableObjectClass) && PoolableObjectClass->ImplementsInterface(UPRPoolableInterface::StaticClass());
}

int32 UPRBaseObjectPoolSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	int32 NewIndex = 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}
