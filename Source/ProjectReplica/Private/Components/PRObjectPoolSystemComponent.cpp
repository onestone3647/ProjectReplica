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
	DynamicPoolSize = 3;
}

void UPRObjectPoolSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	ClearObjectPool();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPRObjectPoolSystemComponent::InitializeObjectPool()
{
	ClearObjectPool();

	// ObjectPoolSettings 데이터 테이블을 기반으로 ObjectPool을 생성합니다.
	if(ObjectPoolSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = ObjectPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRObjectPoolSettings* ObjectPoolSettings = ObjectPoolSettingsDataTable->FindRow<FPRObjectPoolSettings>(RowName, FString(""));
			if(ObjectPoolSettings != nullptr)
			{
				CreateObjectPool(*ObjectPoolSettings);
			}
		}
	}
}

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObjectFromClass(TSubclassOf<APRPooledObject> PooledObjectClass, FVector NewLocation, FRotator NewRotation)
{
	// 활성화할 오브젝트입니다.
	APRPooledObject* ActivateablePooledObject = ActivatePooledObject(GetActivateablePooledObject(PooledObjectClass), NewLocation, NewRotation);
	if(IsValid(ActivateablePooledObject))
	{
		return ActivateablePooledObject;
	}
	
	return nullptr;
}

APRPooledObject* UPRObjectPoolSystemComponent::ActivatePooledObject(APRPooledObject* PooledObject, FVector NewLocation,	FRotator NewRotation)
{
	// 이미 활성화되었거나 ObjectPool이 생성되있지 않다면 nullptr을 반환합니다.
	if(PooledObject->IsActivate() || !IsCreateObjectPool(PooledObject->GetClass()))
	{
		return nullptr;
	}
	
	// 동적으로 생성한 오브젝트일 경우 DynamicObjectDestroyTimer가 작동 중이라면 DynamicObjectDestroyTimer를 정지합니다.
	FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(PooledObject->GetClass());
	if(DynamicDestroyPair != nullptr)
	{
		FTimerHandle* DynamicObjectDestroyTimer = DynamicDestroyPair->Objects.Find(PooledObject);
		if(DynamicObjectDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicObjectDestroyTimer);
		}
	}

	// 오브젝트를 Spawn할 위치와 회전 값을 적용하고 활성화합니다.
	PooledObject->SetActorLocationAndRotation(NewLocation, NewRotation);
	PooledObject->Activate();
	
	// 해당 오브젝트 클래스를 처음 활성화하는 경우 ActivateObjectIndexList를 생성합니다.
	if(!IsCreateActivateObjectIndexList(PooledObject->GetClass()))
	{
		CreateActivateObjectIndexList(PooledObject->GetClass());
	}

	// 활성화된 오브젝트의 Index를 ActivateObjectIndexList에 추가합니다.
	ActivateObjectIndexList.Find(PooledObject->GetClass())->Indexes.Add(PooledObject->GetPoolIndex());
	
	return PooledObject;
}

APRPooledObject* UPRObjectPoolSystemComponent::GetActivateablePooledObject(TSubclassOf<APRPooledObject> PooledObjectClass)
{
	// 해당 오브젝트 클래스의 ObjectPool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateObjectPool(PooledObjectClass))
	{
		FPRObjectPoolSettings ObjectPoolSettings = FPRObjectPoolSettings(PooledObjectClass, DynamicPoolSize);
		CreateObjectPool(ObjectPoolSettings);
	}

	// ObjectPool에서 해당 오브젝트 클래스의 Pool을 얻습니다.
	FPRObjectPool* Pair = ObjectPool.Find(PooledObjectClass);
	if(Pair == nullptr)
	{
		// 지정된 오브젝트 클래스가 없습니다.
		return nullptr;
	}
	
	// 활성화할 오브젝트입니다.
	APRPooledObject* ActivateablePooledObject = nullptr;

	// Pair에서 활성화 되지 않은 오브젝트를 찾습니다.
	for(const auto& PooledObject : Pair->Objects)
	{
		if(IsValid(PooledObject) && !IsActivatePooledObject(PooledObject))
		{
			ActivateablePooledObject = PooledObject;
		
			break;
		}
	}

	if(IsValid(ActivateablePooledObject))
	{
		// 활성화할 오브젝트가 있을 경우 초기화합니다.
		
		int32 Index = ActivateablePooledObject->GetPoolIndex();
		
		// 오브젝트를 초기화하고 OnPooledObjectDeactivate 함수를 바인딩합니다.
		ActivateablePooledObject->InitializeObject(GetOwner(), Index);
		ActivateablePooledObject->OnPooledObjectDeactivateDelegate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnPooledObjectDeactivate);
	}
	else
	{
		// Pair의 모든 오브젝트가 활성화 되었거나 활성화할 오브젝트가 없을 경우 새로운 오브젝트를 생성합니다.
		
		// Critical Section 시작
		FCriticalSection CriticalSection;
		CriticalSection.Lock();

		// 해당 오브젝트 클래스의 UsedObjectIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
		if(!IsCreateUsedObjectIndexList(PooledObjectClass))
		{
			FPRUsedIndexList UsedIndexList;
			for(const auto& PooledObject : Pair->Objects)
			{
				if(IsValid(PooledObject))
				{
					UsedIndexList.Indexes.Add(PooledObject->GetPoolIndex());
				}
			}

			UsedObjectIndexList.Emplace(PooledObjectClass, UsedIndexList);
		}

		// UsedObjectIndexList에서 해당 오브젝트 클래스의 UsedIndexList를 얻습니다.
		FPRUsedIndexList* UsedPooledObjectIndexList = UsedObjectIndexList.Find(PooledObjectClass);
		if(UsedPooledObjectIndexList == nullptr)
		{
			// 지정된 오브젝트 클래스가 없습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(UsedPooledObjectIndexList->Indexes);
		// 사용 가능한 Index를 UsedPooledObjectIndexList에 추가합니다.
		UsedPooledObjectIndexList->Indexes.Add(NewIndex);

		// Critical Section 끝
		CriticalSection.Unlock();

		// 새로운 오브젝트를 생성하고 사용 가능한 Index로 초기화합니다.
		ActivateablePooledObject = SpawnAndInitializeObject(PooledObjectClass, NewIndex);
		
		// OnDynamicObjectDeactivate 함수를 바인딩합니다.
		ActivateablePooledObject->OnPooledObjectDeactivateDelegate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate);

		// 새로 생성한 오브젝트를 Pair에 추가합니다.
		Pair->Objects.Emplace(ActivateablePooledObject);
	}
	
	return ActivateablePooledObject;
}

bool UPRObjectPoolSystemComponent::IsActivatePooledObject(APRPooledObject* PooledObject) const
{
	if(!IsValid(PooledObject))
	{
		return false;
	}
	
	const FPRActivateIndexList* ActivateObjectIndexes = ActivateObjectIndexList.Find(PooledObject->GetClass());
	if(ActivateObjectIndexes != nullptr)
	{
		return ActivateObjectIndexes->Indexes.Contains(PooledObject->GetPoolIndex()) || PooledObject->IsActivate();
	}
	
	return false;
}

bool UPRObjectPoolSystemComponent::IsCreateObjectPool(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ObjectPool.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateActivateObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return ActivateObjectIndexList.Contains(PooledObjectClass);
}

bool UPRObjectPoolSystemComponent::IsCreateUsedObjectIndexList(TSubclassOf<APRPooledObject> PooledObjectClass) const
{
	return UsedObjectIndexList.Contains(PooledObjectClass);
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnObjectInWorld(TSubclassOf<APRPooledObject> ObjectClass)
{
	if(GetWorld() != nullptr)
	{
		return GetWorld()->SpawnActor<APRPooledObject>(ObjectClass);
	}

	return nullptr;
}

APRPooledObject* UPRObjectPoolSystemComponent::SpawnAndInitializeObject(TSubclassOf<APRPooledObject> ObjectClass, int32 Index)
{
	TObjectPtr<APRPooledObject> SpawnObject = SpawnObjectInWorld(ObjectClass);
	if(SpawnObject != nullptr)
	{
		// 생성한 오브젝트를 초기화하고 OnPooledObjectDeactivate 함수를 바인딩합니다.
		SpawnObject->InitializeObject(GetOwner(), Index);
		SpawnObject->OnPooledObjectDeactivateDelegate.AddDynamic(this, &UPRObjectPoolSystemComponent::OnPooledObjectDeactivate);
	}

	return SpawnObject;
}

int32 UPRObjectPoolSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	int32 NewIndex = 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}

void UPRObjectPoolSystemComponent::ClearObjectPool()
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
	}
	ObjectPool.Empty();
}

void UPRObjectPoolSystemComponent::CreateObjectPool(FPRObjectPoolSettings ObjectPoolSettings)
{
	if(GetWorld() != nullptr && ObjectPoolSettings.PooledObjectClass != nullptr)
	{
		// ObjectPool에 추가할 Pair를 초기화하고 오브젝트를 생성하여 추가합니다.
		FPRObjectPool Pair;
		for(int32 Index = 0; Index < ObjectPoolSettings.PoolSize; Index++)
		{
			// 오브젝트를 월드에 Spawn하고 초기화합니다.
			APRPooledObject* SpawnObject = SpawnAndInitializeObject(ObjectPoolSettings.PooledObjectClass, Index);
			if(SpawnObject != nullptr)
			{
				Pair.Objects.Emplace(SpawnObject);
			}
		}

		// 초기화된 Pair를 ObjectPool에 추가하고 ActivateObjectIndexList를 생성합니다.
		ObjectPool.Emplace(ObjectPoolSettings.PooledObjectClass, Pair);
		CreateActivateObjectIndexList(ObjectPoolSettings.PooledObjectClass);
	}
}

void UPRObjectPoolSystemComponent::CreateActivateObjectIndexList(TSubclassOf<APRPooledObject> ObjectClass)
{
	// ObjectClass의 ActivateObjectIndexList를 생성합니다.
	ActivateObjectIndexList.Emplace(ObjectClass);
}

void UPRObjectPoolSystemComponent::OnPooledObjectDeactivate(APRPooledObject* PooledObject)
{
	if(!IsValid(PooledObject))
	{
		return;
	}
	
	if(IsActivatePooledObject(PooledObject))
	{
		FPRActivateIndexList* IndexList = ActivateObjectIndexList.Find(PooledObject->GetClass());
		if(IndexList != nullptr)
		{
			// 비활성화된 오브젝트의 Index를 ActivateObjectIndexList에서 제거합니다.
			IndexList->Indexes.Remove(PooledObject->GetPoolIndex());
		}
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDeactivate(APRPooledObject* PooledObject)
{
	if(DynamicDestroyDelay > 0.0f)
	{
		// 딜레이 시간이 0보다 클 경우, 즉 딜레이 시간이 지난 후 동적으로 생성한 오브젝트를 제거할 경우

		// 타이머를 설정합니다.
		FTimerHandle DynamicDestroyDelayTimerHandle;
		FTimerDelegate DynamicDestroyDelayDelegate = FTimerDelegate::CreateUObject(this, &UPRObjectPoolSystemComponent::OnDynamicObjectDestroy, PooledObject);
		GetWorld()->GetTimerManager().SetTimer(DynamicDestroyDelayTimerHandle, DynamicDestroyDelayDelegate, DynamicDestroyDelay, false);

		// TimerHandle을 DynamicDestroyObjectList에 추가합니다.
		FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(PooledObject->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			// 해당 오브젝트 클래스로 생성된 DynamicDestroyObjectList가 있을 경우
			DynamicDestroyPair->Objects.Emplace(PooledObject, DynamicDestroyDelayTimerHandle);
		}
		else
		{
			// 해당 오브젝트 클래스로 생성된 DynamicDestroyObjectList가 없을 경우
			FPRDynamicDestroyObjectList DynamicObjectList;
			DynamicObjectList.Objects.Emplace(PooledObject, DynamicDestroyDelayTimerHandle);
			DynamicDestroyObjectList.Emplace(PooledObject->GetClass(), DynamicObjectList);
		}
	}
	else
	{
		// 딜레이 시간이 0보다 작거나 같을 경우, 즉 동적으로 생성한 오브젝트를 바로 제거할 경우
		OnDynamicObjectDestroy(PooledObject);		
	}
}

void UPRObjectPoolSystemComponent::OnDynamicObjectDestroy(APRPooledObject* PooledObject)
{
	// 사용 중인 ObjectPoolIndexList를 가지고 있는지 확인합니다.
	if(IsCreateUsedObjectIndexList(PooledObject->GetClass()))
	{
		// DynamicObjectDestroyTimer를 제거합니다.
		FPRDynamicDestroyObjectList* DynamicDestroyPair = DynamicDestroyObjectList.Find(PooledObject->GetClass());
		if(DynamicDestroyPair != nullptr)
		{
			DynamicDestroyPair->Objects.Remove(PooledObject);
		}

		// 오브젝트 클래스의 UsedObjectIndex를 얻습니다.
		FPRUsedIndexList* UsedIndexList = UsedObjectIndexList.Find(PooledObject->GetClass());
		if(UsedIndexList != nullptr)
		{
			// 사용 중인 Index를 제거합니다.
			UsedIndexList->Indexes.Remove(PooledObject->GetPoolIndex());
		}

		// 오브젝트 클래스의 ObjectPool이 생성되었는지 확인합니다.
		if(IsCreateObjectPool(PooledObject->GetClass()))
		{
			// ObjectPool에서 오브젝트를 제거하고 초기화한 후 파괴합니다.
			ObjectPool.Find(PooledObject->GetClass())->Objects.Remove(PooledObject);
			PooledObject->InitializeObject();
			PooledObject->Destroy();
		}
	}
}
