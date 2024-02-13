// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRObjectPoolSystemComponent.h"
#include "Objects/PRPooledObject.h"

UPRObjectPoolSystemComponent::UPRObjectPoolSystemComponent()
{
	ObjectPool.Empty();
	ActivateObjectIndexList.Empty();
	UsedObjectIndexList.Empty();
	DynamicDestroyObjectList.Empty();
	DynamicDestroyDelay = 60.0f; 
}

void UPRObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	ActivateObjectIndexList.Empty();
	UsedObjectIndexList.Empty();
	DynamicDestroyObjectList.Empty();
	
	// 모든 ObjectPool을 제거합니다.
	if(ObjectPool.Num() > 0)
	{
		for(auto& Pair : ObjectPool)
		{
			for(auto& PooledObject : Pair.Value.Objects)
			{
				PooledObject->Destroy();
			}
		}

		ObjectPool.Empty();
	}
	
	Super::DestroyComponent(bPromoteChildren);
}
