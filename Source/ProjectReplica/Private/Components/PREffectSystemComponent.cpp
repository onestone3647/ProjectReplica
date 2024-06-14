// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PREffectSystemComponent.h"
#include "Characters/PRBaseCharacter.h"

UPREffectSystemComponent::UPREffectSystemComponent()
{
	// NiagaraSystem
	NiagaraPoolSettingsDataTable = nullptr;
	NiagaraPool = FPRNiagaraEffectObjectPool();
	ActivateNiagaraIndexList = FPRActivateNiagaraEffectIndexList();
	UsedNiagaraIndexList = FPRUsedNiagaraEffectIndexList();
	DynamicDestroyNiagaraList = FPRDynamicDestroyNiagaraEffectList();

	// ParticleSystem
	ParticlePoolSettingsDataTable = nullptr;
	ParticlePool = FPRParticleEffectObjectPool();
	ActivateParticleIndexList = FPRActivateParticleEffectIndexList();
	UsedParticleIndexList = FPRUsedParticleEffectIndexList();
	DynamicDestroyParticleList = FPRDynamicDestroyParticleEffectList();
}

#pragma region PRBaseObjectPoolSystem
void UPREffectSystemComponent::InitializeObjectPool()
{
	InitializeNiagaraPool();
	InitializeParticlePool();
}

void UPREffectSystemComponent::ClearAllObjectPool()
{
	ClearAllNiagaraPool();
	ClearAllParticlePool();
}
#pragma endregion 

#pragma region NiagaraSystem
void UPREffectSystemComponent::InitializeNiagaraPool()
{
	ClearAllNiagaraPool();
	
	// NiagaraSystemPoolSettings 데이터 테이블을 기반으로 NiagaraSystemObjectPool을 생성합니다.
	if(NiagaraPoolSettingsDataTable)
	{
		TArray<FName> RowNames = NiagaraPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRNiagaraEffectPoolSettings* NiagaraSettings = NiagaraPoolSettingsDataTable->FindRow<FPRNiagaraEffectPoolSettings>(RowName, FString(""));
			if(NiagaraSettings)
			{
				CreateNiagaraPool(*NiagaraSettings);
			}
		}
	}
}

void UPREffectSystemComponent::ClearAllNiagaraPool()
{
	ActivateNiagaraIndexList.List.Empty();
	UsedNiagaraIndexList.List.Empty();
	ClearDynamicDestroyNiagaraList(DynamicDestroyNiagaraList);
	ClearNiagaraPool(NiagaraPool);
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = InitializeNiagaraEffect(SpawnEffect);
	if(IsValid(ActivateableNiagaraEffect))
	{
		// NiagaraEffect를 활성화하고 Spawn할 위치와 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate, bReset);
	
		return ActivateableNiagaraEffect;
	}

	return nullptr;
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect,	USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = InitializeNiagaraEffect(SpawnEffect);
	if(IsValid(ActivateableNiagaraEffect))
	{
		// NiagaraEffect를 활성화하고 Spawn하여 부착할 Component와 위치, 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAttached(Parent, AttachSocketName, Location, Rotation, Scale, EAttachLocation::KeepWorldPosition, bEffectAutoActivate, bReset);
	
		return ActivateableNiagaraEffect;
	}

	return nullptr;
}

APRNiagaraEffect* UPREffectSystemComponent::GetActivateableNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
	// NiagaraSystem이 유효하지 않을 경우 nullptr을 반환합니다.
	if(!NiagaraSystem)
	{
		return nullptr;
	}

	// 해당 NiagaraSystem에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateNiagaraPool(NiagaraSystem))
	{
		FPRNiagaraEffectPoolSettings NiagaraPoolSettings = FPRNiagaraEffectPoolSettings(NiagaraSystem, DynamicPoolSize, DynamicLifespan);
		CreateNiagaraPool(NiagaraPoolSettings);
	}

	// NiagaraPool에서 해당 NiagaraSystem의 Pool을 얻습니다.
	FPRNiagaraEffectPool* PoolEntry = NiagaraPool.Pool.Find(NiagaraSystem);
	if(!PoolEntry)
	{
		// 지정된 NiagaraSystem가 없으면 nullptr을 반환합니다.
		return nullptr;
	}

	// 활성화할 NiagaraEffect입니다.	
	APRNiagaraEffect* ActivateableNiagaraEffect = nullptr;

	// PoolEntry에서 활성화되지 않은 NiagaraEffect를 찾습니다.
	for(const auto& NiagaraEffect : PoolEntry->PooledEffects)
	{
		if(!IsActivateNiagaraEffect(NiagaraEffect))
		{
			ActivateableNiagaraEffect = NiagaraEffect;

			break;
		}
	}

	// PoolEntry의 모든 NiagaraEffect가 활성화되었을 경우 새로운 NiagaraEffect를 생성합니다.
	if(!ActivateableNiagaraEffect)
	{
		ActivateableNiagaraEffect = SpawnDynamicNiagaraEffectInWorld(NiagaraSystem);
	}
	
	// 동적으로 생성된 NiagaraEffect일 경우 DynamicEffectDestroyTimer를 정지합니다.
	if(IsDynamicNiagaraEffect(ActivateableNiagaraEffect))
	{
		FPRDynamicDestroyObject* DynamicNiagaraList = DynamicDestroyNiagaraList.List.Find(ActivateableNiagaraEffect->GetNiagaraEffectAsset());
		if(DynamicNiagaraList)
		{
			FTimerHandle* DynamicDestroyTimer = DynamicNiagaraList->TimerHandles.Find(ActivateableNiagaraEffect);
			if(DynamicDestroyTimer)
			{
				GetWorld()->GetTimerManager().ClearTimer(*DynamicDestroyTimer);
			}
		}
	}
	
	return ActivateableNiagaraEffect;
}

bool UPREffectSystemComponent::IsActivateNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const
{
	// 유효하지 않는 NiagaraEffect이거나 풀링 가능한 객체가 아니면 false를 반환합니다.
	if(!IsValid(NiagaraEffect) || !IsPoolableObject(NiagaraEffect))
	{
		return false;
	}

	// NiagaraSystem에 해당하는 활성화된 Index 목록을 찾습니다.
	const FPRActivateIndexList* IndexList = ActivateNiagaraIndexList.List.Find(NiagaraEffect->GetNiagaraEffectAsset());
	if(IndexList)
	{
		// 객체의 PoolIndex를 가져오고 객체가 활성화된 상태인지 확인합니다.
		const int32 PooledIndex = GetPoolIndex(NiagaraEffect);
		const bool bIsNiagaraEffectActivated = IsActivateObject(NiagaraEffect);

		// Index 목록에 해당 Index가 포함되어 있고, 객체가 활성화된 상태이면 true를 반환합니다.
		return IndexList->Indexes.Contains(PooledIndex) && bIsNiagaraEffectActivated;
	}

	// 위 조건을 모두 만족하지 않으면 false를 반환합니다.
	return false;
}

bool UPREffectSystemComponent::IsCreateNiagaraPool(UNiagaraSystem* NiagaraSystem) const
{
	return NiagaraPool.Pool.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsCreateActivateNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return ActivateNiagaraIndexList.List.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsCreateUsedNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return UsedNiagaraIndexList.List.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsDynamicNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const
{
	// 주어진 객체가 유효한 풀링 가능한 객체인지 확인합니다.
	if(!IsPoolableObject(NiagaraEffect))
	{
		return false;
	}

	const FPRDynamicDestroyObject* DynamicNiagaraList = DynamicDestroyNiagaraList.List.Find(NiagaraEffect->GetNiagaraEffectAsset());
	if(DynamicNiagaraList)
	{
		return DynamicNiagaraList->TimerHandles.Contains(NiagaraEffect);
	}
	
	return false;
}

void UPREffectSystemComponent::ClearNiagaraPool(FPRNiagaraEffectObjectPool& TargetNiagaraPool)
{
	// NiagaraPool을 제거합니다.
	for(auto& PoolEntry : NiagaraPool.Pool)
	{
		FPRNiagaraEffectPool& Pool = PoolEntry.Value;
		for(auto& PooledEffect : Pool.PooledEffects)
		{
			if(IsValid(PooledEffect))
			{
				// Effect를 제거합니다.
				PooledEffect->ConditionalBeginDestroy();
				PooledEffect = nullptr;
			}
		}

		Pool.PooledEffects.Empty();
	}

	NiagaraPool.Pool.Empty();
}

void UPREffectSystemComponent::CreateNiagaraPool(const FPRNiagaraEffectPoolSettings& NiagaraPoolSettings)
{
	if(GetWorld() && NiagaraPoolSettings.NiagaraSystem)
	{
		FPRNiagaraEffectPool NewNiagaraEffectPool;

		// PoolSize만큼 NiagaraEffect를 월드에 Spawn한 후 NewNiagaraEffectPool에 보관합니다.
		for(int32 Index = 0; Index < NiagaraPoolSettings.PoolSize; Index++)
		{
			APRNiagaraEffect* SpawnNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraPoolSettings.NiagaraSystem, Index, NiagaraPoolSettings.EffectLifespan);
			if(IsValid(SpawnNiagaraEffect))
			{
				NewNiagaraEffectPool.PooledEffects.Emplace(SpawnNiagaraEffect);
			}
		}

		// 초기화된 NewNiagaraEffectPool를 NiagaraPool에 추가하고 ActivateNiagaraIndexList를 생성합니다.
		NiagaraPool.Pool.Emplace(NiagaraPoolSettings.NiagaraSystem, NewNiagaraEffectPool);
	}
}

void UPREffectSystemComponent::CreateActivateNiagaraIndexList(UNiagaraSystem* NiagaraSystem)
{
	if(NiagaraSystem)
	{
		ActivateNiagaraIndexList.List.Emplace(NiagaraSystem);
	}
}

void UPREffectSystemComponent::CreateUsedNiagaraIndexList(UNiagaraSystem* NiagaraSystem)
{
	if(NiagaraSystem)
	{
		FPRNiagaraEffectPool* PoolEntry = NiagaraPool.Pool.Find(NiagaraSystem);
		if(PoolEntry)
		{
			FPRUsedIndexList UsedIndexList;
			for(const auto& PooledEffect : PoolEntry->PooledEffects)
			{
				if(IsValid(PooledEffect))
				{
					UsedIndexList.Indexes.Add(GetPoolIndex(PooledEffect));
				}
			}
			
			UsedNiagaraIndexList.List.Emplace(NiagaraSystem, UsedIndexList);	
		}
	}
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem, int32 PoolIndex, float Lifespan)
{
	if(!GetWorld() || !NiagaraSystem || !GetPROwner())
	{
		return nullptr;
	}

	// NiagaraEffect를 생성합니다.
	APRNiagaraEffect* NiagaraEffect = GetWorld()->SpawnActor<APRNiagaraEffect>(APRNiagaraEffect::StaticClass());
	if(!IsValid(NiagaraEffect))
	{
		// NiagaraEffect 생성에 실패하면 함수를 종료하고 nullptr을 반환합니다.
		return nullptr;
	}

	// NiagaraEffect를 초기화합니다.
	NiagaraEffect->InitializeNiagaraEffect(NiagaraSystem, GetPROwner(), PoolIndex, Lifespan);

	// NiagaraEffect의 OnEffectDeactivateDelegate 이벤트에 대한 콜백 함수를 바인딩합니다.
	NiagaraEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnNiagaraEffectDeactivate);

	return NiagaraEffect;
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnDynamicNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem)
{
	if(!NiagaraSystem)
	{
		return nullptr;
	}
	
	APRNiagaraEffect* DynamicNiagaraEffect = nullptr;
	
	// Critical Section 시작
	FCriticalSection CriticalSection;
	CriticalSection.Lock();

	// 해당 NiagaraSystem의 UsedNiagaraIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateUsedNiagaraIndexList(NiagaraSystem))
	{
		CreateUsedNiagaraIndexList(NiagaraSystem);
	}

	// UsedNiagaraIndexList에서 해당 NiagaraSystem의 UsedIndexList를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedNiagaraIndexList.List.Find(NiagaraSystem);
	if(UsedIndexList == nullptr)
	{
		// 지정된 NiagaraSystem이 없습니다.
		return nullptr;
	}

	// 사용 가능한 Index를 구합니다.
	const int32 NewIndex = FindAvailableIndex(UsedIndexList->Indexes);
	// 사용 가능한 Index를 UsedIndexList에 추가합니다.
	UsedIndexList->Indexes.Add(NewIndex);

	// Critical Section 끝
	CriticalSection.Unlock();

	// 새로운 NiagaraEffect를 생성하고 초기화합니다.
	const FPRNiagaraEffectPoolSettings NiagaraEffectSettings = GetNiagaraEffectPoolSettingsFromDataTable(NiagaraSystem);
	if(NiagaraEffectSettings != FPRNiagaraEffectPoolSettings())
	{
		// 데이터 테이블에 NiagaraEffect의 설정 값을 가지고 있을 경우 설정 값의 Lifespan을 적용합니다.
		DynamicNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraSystem, NewIndex, NiagaraEffectSettings.EffectLifespan);
	}
	else
	{
		// 데이터 테이블에 NiagaraEffect의 설정 값을 가지고 있지 않을 경우 DynamicLifespan을 적용합니다.
		DynamicNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraSystem, NewIndex, DynamicLifespan);
	}
		
	// OnDynamicNiagaraEffectDeactivate 함수를 바인딩합니다.
	DynamicNiagaraEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate);

	// NiagaraPool에서 해당 NiagaraSystem의 Pool을 얻습니다.
	FPRNiagaraEffectPool* PoolEntry = NiagaraPool.Pool.Find(NiagaraSystem);
	if(!PoolEntry)
	{
		// Pool이 없을 경우 생성한 NiagaraEffect를 제거하고 nullptr을 반환합니다.
		DynamicNiagaraEffect->ConditionalBeginDestroy();
		
		return nullptr;
	}

	// 새로 생성한 NiagaraEffect를 PoolEntry에 추가합니다.
	PoolEntry->PooledEffects.Emplace(DynamicNiagaraEffect);

	return DynamicNiagaraEffect;
}

APRNiagaraEffect* UPREffectSystemComponent::InitializeNiagaraEffect(UNiagaraSystem* SpawnEffect)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = GetActivateableNiagaraEffect(SpawnEffect);
	
	// 유효하지 않는 NiagaraEffect이거나 풀링 가능한 객체가 아니면 nullptr를 반환합니다.
	if(!IsValid(ActivateableNiagaraEffect) || !IsPoolableObject(ActivateableNiagaraEffect))
	{
		return nullptr;
	}

	// 동적으로 생성한 NiagaraEffect일 경우 DynamicObjectDestroyTimer가 작동 중이라면 DynamicObjectDestroyTimer를 정지합니다.
	FTimerHandle* DynamicObjectDestroyTimer = DynamicDestroyNiagaraList.FindTimerHandleForNiagaraEffect(*ActivateableNiagaraEffect);
	if(DynamicObjectDestroyTimer)
	{
		GetWorld()->GetTimerManager().ClearTimer(*DynamicObjectDestroyTimer);
	}
	
	// 해당 NiagaraSystem를 처음 활성화하는 경우 ActivateNiagaraEffectIndexList를 생성합니다.
	if(!IsCreateActivateNiagaraIndexList(SpawnEffect))
	{
		CreateActivateNiagaraIndexList(SpawnEffect);
	}

	// 활성화된 NiagaraEffect의 Index를 ActivateNiagaraIndexList에 저장합니다.
	const int32 PoolIndex = GetPoolIndex(ActivateableNiagaraEffect);
	ActivateNiagaraIndexList.GetIndexesForNiagaraSystem(*SpawnEffect)->Add(PoolIndex);
	
	return ActivateableNiagaraEffect;
}

void UPREffectSystemComponent::ClearDynamicDestroyNiagaraList(FPRDynamicDestroyNiagaraEffectList& TargetDynamicDestroyNiagaraEffectList)
{
	ClearDynamicDestroyObjects(TargetDynamicDestroyNiagaraEffectList.List);
}

FPRNiagaraEffectPoolSettings UPREffectSystemComponent::GetNiagaraEffectPoolSettingsFromDataTable(UNiagaraSystem* NiagaraSystem) const
{
	if(NiagaraPoolSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = NiagaraPoolSettingsDataTable->GetRowNames();
		for(const FName& RowName: RowNames)
		{
			FPRNiagaraEffectPoolSettings* NiagaraEffectPoolSettings = NiagaraPoolSettingsDataTable->FindRow<FPRNiagaraEffectPoolSettings>(RowName, FString(""));
			if(NiagaraEffectPoolSettings
				&& NiagaraEffectPoolSettings->NiagaraSystem == NiagaraSystem)
			{
				return *NiagaraEffectPoolSettings;
			}
		}
	}

	return FPRNiagaraEffectPoolSettings();
}

void UPREffectSystemComponent::OnNiagaraEffectDeactivate(APREffect* TargetEffect)
{
	APRNiagaraEffect* TargetNiagaraEffect = Cast<APRNiagaraEffect>(TargetEffect);
	// 유효하지 않는 NiagaraEffect이거나 풀링 가능한 NiagaraEffect가 아니거나 NiagaraComponent가 없으면 반환합니다.
	if(!IsValid(TargetNiagaraEffect)
		|| !IsPoolableObject(TargetNiagaraEffect)
		|| !TargetNiagaraEffect->GetNiagaraEffect())
	{
		return;
	}

	// TargetNiagaraEffect가 활성화된 상태라면 비활성화합니다.
	if(IsActivateNiagaraEffect(TargetNiagaraEffect))
	{
		FPRActivateIndexList* ActivateIndexList = ActivateNiagaraIndexList.List.Find(TargetNiagaraEffect->GetNiagaraEffectAsset());
		if(ActivateIndexList)
		{
			// 비활성화된 TargetNiagaraEffect의 Index를 제거합니다.
			ActivateIndexList->Indexes.Remove(GetPoolIndex(TargetNiagaraEffect));
		}
	}
}

void UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate(APREffect* TargetEffect)
{
	APRNiagaraEffect* TargetNiagaraEffect = Cast<APRNiagaraEffect>(TargetEffect);
	// 유효하지 않는 NiagaraEffect이거나 풀링 가능한 NiagaraEffect가 아니거나 NiagaraComponent가 없으면 반환합니다.
	if(!IsValid(TargetNiagaraEffect)
		|| !IsPoolableObject(TargetNiagaraEffect)
		|| !TargetNiagaraEffect->GetNiagaraEffect())
	{
		return;
	}

	if(DynamicLifespan > 0.0f)
	{
		// 동적 수명이 끝난 후 NiagaraEffect를 제거하도록 타이머를 설정합니다.
		FTimerHandle DynamicLifespanTimerHandle;
		FTimerDelegate DynamicLifespanDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::OnDynamicNiagaraEffectDestroy, TargetNiagaraEffect);
		GetWorld()->GetTimerManager().SetTimer(DynamicLifespanTimerHandle, DynamicLifespanDelegate, DynamicLifespan, false);

		// TimerHandle을 추가합니다.
		FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyNiagaraList.List.Find(TargetNiagaraEffect->GetNiagaraEffectAsset());
		if(DynamicDestroyObject)
		{
			DynamicDestroyObject->TimerHandles.Emplace(TargetNiagaraEffect, DynamicLifespanTimerHandle);
		}
		else
		{
			FPRDynamicDestroyObject NewDynamicDestroyObject;
			NewDynamicDestroyObject.TimerHandles.Emplace(TargetNiagaraEffect, DynamicLifespanTimerHandle);
			DynamicDestroyNiagaraList.List.Emplace(TargetNiagaraEffect->GetNiagaraEffectAsset(), NewDynamicDestroyObject);
		}
	}
	else
	{
		// 동적 수명이 없을 경우 타이머를 실행하지 않고 바로 NiagaraEffect를 제거합니다.
		OnDynamicNiagaraEffectDestroy(TargetNiagaraEffect);
	}
}

void UPREffectSystemComponent::OnDynamicNiagaraEffectDestroy(APRNiagaraEffect* TargetNiagaraEffect)
{
	// DynamicObjectDestroyTimer를 제거합니다.
	FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyNiagaraList.List.Find(TargetNiagaraEffect->GetNiagaraEffectAsset());
	if(DynamicDestroyObject)
	{
		FTimerHandle* TimerHandle = DynamicDestroyObject->TimerHandles.Find(TargetNiagaraEffect);
		GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
		DynamicDestroyObject->TimerHandles.Remove(TargetNiagaraEffect);
	}

	// NiagaraSystem의 UsedObjectIndex를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedNiagaraIndexList.List.Find(TargetNiagaraEffect->GetNiagaraEffectAsset());
	if(UsedIndexList)
	{
		// 사용 중인 Index를 제거합니다.
		UsedIndexList->Indexes.Remove(GetPoolIndex(TargetNiagaraEffect));
	}

	// NiagaraSystem의 Pool이 생성되었는지 확인합니다.
	if(IsCreateNiagaraPool(TargetNiagaraEffect->GetNiagaraEffectAsset()))
	{
		// NiagaraPool에서 NiagaraEffect를 제거합니다.
		FPRNiagaraEffectPool* PoolEntry = NiagaraPool.Pool.Find(TargetNiagaraEffect->GetNiagaraEffectAsset());
		if(PoolEntry)
		{
			PoolEntry->PooledEffects.Remove(TargetNiagaraEffect);
		}
	}
		
	TargetNiagaraEffect->ConditionalBeginDestroy();
}
#pragma endregion 

#pragma region ParticleSystem
void UPREffectSystemComponent::InitializeParticlePool()
{
	ClearAllParticlePool();
	
	// ParticleSystemPoolSettings 데이터 테이블을 기반으로 ParticleSystemObjectPool을 생성합니다.
	if(ParticlePoolSettingsDataTable)
	{
		TArray<FName> RowNames = ParticlePoolSettingsDataTable->GetRowNames();
		for(const FName& RowName : RowNames)
		{
			FPRParticleEffectPoolSettings* ParticleSettings = ParticlePoolSettingsDataTable->FindRow<FPRParticleEffectPoolSettings>(RowName, FString(""));
			if(ParticleSettings)
			{
				CreateParticlePool(*ParticleSettings);
			}
		}
	}
}

void UPREffectSystemComponent::ClearAllParticlePool()
{
	ActivateParticleIndexList.List.Empty();
	UsedParticleIndexList.List.Empty();
	ClearDynamicDestroyParticleList(DynamicDestroyParticleList);
	ClearParticlePool(ParticlePool);
}

APRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
{
	APRParticleEffect* ActivateableParticleEffect = InitializeParticleEffect(SpawnEffect);
	if(IsValid(ActivateableParticleEffect))
	{
		return nullptr;
	}
	
	// ParticleEffect를 활성화하고 Spawn할 위치와 회전값, 크기, 자동실행 여부를 적용합니다.
	ActivateableParticleEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate, bReset);
	
	return ActivateableParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAttached(UParticleSystem* SpawnEffect,	USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
{
	APRParticleEffect* ActivateableParticleEffect = InitializeParticleEffect(SpawnEffect);
	if(IsValid(ActivateableParticleEffect))
	{
		return nullptr;
	}

	// ParticleEffect를 활성화하고 Spawn하여 부착할 Component와 위치, 회전값, 크기, 자동실행 여부를 적용합니다.
	ActivateableParticleEffect->SpawnEffectAttached(Parent, AttachSocketName, Location, Rotation, Scale, EAttachLocation::KeepWorldPosition, bEffectAutoActivate, bReset);
	
	return ActivateableParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::GetActivateableParticleEffect(UParticleSystem* ParticleSystem)
{
	// ParticleSystem이 유효하지 않을 경우 nullptr을 반환합니다.
	if(!ParticleSystem)
	{
		return nullptr;
	}

	// 해당 ParticleSystem에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateParticlePool(ParticleSystem))
	{
		FPRParticleEffectPoolSettings ParticlePoolSettings = FPRParticleEffectPoolSettings(ParticleSystem, DynamicPoolSize, DynamicLifespan);
		CreateParticlePool(ParticlePoolSettings);
	}

	// ParticlePool에서 해당 ParticleSystem의 Pool을 얻습니다.
	FPRParticleEffectPool* PoolEntry = ParticlePool.Pool.Find(ParticleSystem);
	if(!PoolEntry)
	{
		// 지정된 ParticleSystem가 없으면 nullptr을 반환합니다.
		return nullptr;
	}

	// 활성화할 ParticleEffect입니다.	
	APRParticleEffect* ActivateableParticleEffect = nullptr;

	// PoolEntry에서 활성화되지 않은 ParticleEffect를 찾습니다.
	for(const auto& ParticleEffect : PoolEntry->PooledEffects)
	{
		if(!IsActivateParticleEffect(ParticleEffect))
		{
			ActivateableParticleEffect = ParticleEffect;

			break;
		}
	}

	// PoolEntry의 모든 ParticleEffect가 활성화되었을 경우 새로운 ParticleEffect를 생성합니다.
	if(!ActivateableParticleEffect)
	{
		ActivateableParticleEffect = SpawnDynamicParticleEffectInWorld(ParticleSystem);
	}
	
	// 동적으로 생성된 ParticleEffect일 경우 DynamicEffectDestroyTimer를 정지합니다.
	if(IsDynamicParticleEffect(ActivateableParticleEffect))
	{
		FPRDynamicDestroyObject* DynamicParticleList = DynamicDestroyParticleList.List.Find(ActivateableParticleEffect->GetParticleEffectAsset());
		if(DynamicParticleList)
		{
			FTimerHandle* DynamicDestroyTimer = DynamicParticleList->TimerHandles.Find(ActivateableParticleEffect);
			if(DynamicDestroyTimer)
			{
				GetWorld()->GetTimerManager().ClearTimer(*DynamicDestroyTimer);
			}
		}
	}
	
	return ActivateableParticleEffect;
}

bool UPREffectSystemComponent::IsActivateParticleEffect(APRParticleEffect* ParticleEffect) const
{
	// 유효하지 않는 ParticleEffect이거나 풀링 가능한 객체가 아니면 false를 반환합니다.
	if(!IsValid(ParticleEffect) || !IsPoolableObject(ParticleEffect))
	{
		return false;
	}

	// ParticleSystem에 해당하는 활성화된 Index 목록을 찾습니다.
	const FPRActivateIndexList* IndexList = ActivateParticleIndexList.List.Find(ParticleEffect->GetParticleEffectAsset());
	if(IndexList)
	{
		// 객체의 PoolIndex를 가져오고 객체가 활성화된 상태인지 확인합니다.
		const int32 PooledIndex = GetPoolIndex(ParticleEffect);
		const bool bIsParticleEffectActivated = IsActivateObject(ParticleEffect);

		// Index 목록에 해당 Index가 포함되어 있고, 객체가 활성화된 상태이면 true를 반환합니다.
		return IndexList->Indexes.Contains(PooledIndex) && bIsParticleEffectActivated;
	}

	// 위 조건을 모두 만족하지 않으면 false를 반환합니다.
	return false;
}

bool UPREffectSystemComponent::IsCreateParticlePool(UParticleSystem* ParticleSystem) const
{
	return ParticlePool.Pool.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsCreateActivateParticleIndexList(UParticleSystem* ParticleSystem) const
{
	return ActivateParticleIndexList.List.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsCreateUsedParticleIndexList(UParticleSystem* ParticleSystem) const
{
	return UsedParticleIndexList.List.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsDynamicParticleEffect(APRParticleEffect* ParticleEffect) const
{
	// 주어진 객체가 유효한 풀링 가능한 객체인지 확인합니다.
	if(!IsPoolableObject(ParticleEffect))
	{
		return false;
	}

	const FPRDynamicDestroyObject* DynamicParticleList = DynamicDestroyParticleList.List.Find(ParticleEffect->GetParticleEffectAsset());
	if(DynamicParticleList)
	{
		return DynamicParticleList->TimerHandles.Contains(ParticleEffect);
	}
	
	return false;
}

void UPREffectSystemComponent::ClearParticlePool(FPRParticleEffectObjectPool& TargetParticlePool)
{
	// ParticlePool을 제거합니다.
	for(auto& PoolEntry : ParticlePool.Pool)
	{
		FPRParticleEffectPool& Pool = PoolEntry.Value;
		for(auto& PooledEffect : Pool.PooledEffects)
		{
			if(IsValid(PooledEffect))
			{
				// Effect를 제거합니다.
				PooledEffect->ConditionalBeginDestroy();
				PooledEffect = nullptr;
			}
		}

		Pool.PooledEffects.Empty();
	}

	ParticlePool.Pool.Empty();
}

void UPREffectSystemComponent::CreateParticlePool(const FPRParticleEffectPoolSettings& ParticlePoolSettings)
{
	if(GetWorld() && ParticlePoolSettings.ParticleSystem)
	{
		FPRParticleEffectPool NewParticleEffectPool;

		// PoolSize만큼 ParticleEffect를 월드에 Spawn한 후 NewParticleEffectPool에 보관합니다.
		for(int32 Index = 0; Index < ParticlePoolSettings.PoolSize; Index++)
		{
			APRParticleEffect* SpawnParticleEffect = SpawnParticleEffectInWorld(ParticlePoolSettings.ParticleSystem, Index, ParticlePoolSettings.EffectLifespan);
			if(IsValid(SpawnParticleEffect))
			{
				NewParticleEffectPool.PooledEffects.Emplace(SpawnParticleEffect);
			}
		}

		// 초기화된 NewParticleEffectPool를 ParticlePool에 추가하고 ActivateParticleIndexList를 생성합니다.
		ParticlePool.Pool.Emplace(ParticlePoolSettings.ParticleSystem, NewParticleEffectPool);
	}
}

void UPREffectSystemComponent::CreateActivateParticleIndexList(UParticleSystem* ParticleSystem)
{
	if(ParticleSystem)
	{
		ActivateParticleIndexList.List.Emplace(ParticleSystem);
	}
}

void UPREffectSystemComponent::CreateUsedParticleIndexList(UParticleSystem* ParticleSystem)
{
	if(ParticleSystem)
	{
		FPRParticleEffectPool* PoolEntry = ParticlePool.Pool.Find(ParticleSystem);
		if(PoolEntry)
		{
			FPRUsedIndexList UsedIndexList;
			for(const auto& PooledEffect : PoolEntry->PooledEffects)
			{
				if(IsValid(PooledEffect))
				{
					UsedIndexList.Indexes.Add(GetPoolIndex(PooledEffect));
				}
			}
			
			UsedParticleIndexList.List.Emplace(ParticleSystem, UsedIndexList);	
		}
	}
}

APRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectInWorld(UParticleSystem* ParticleSystem, int32 PoolIndex, float Lifespan)
{
	if(!GetWorld() || !ParticleSystem || !GetPROwner())
	{
		return nullptr;
	}

	// ParticleEffect를 생성합니다.
	APRParticleEffect* ParticleEffect = GetWorld()->SpawnActor<APRParticleEffect>(APRParticleEffect::StaticClass());
	if(!IsValid(ParticleEffect))
	{
		// ParticleEffect 생성에 실패하면 함수를 종료하고 nullptr을 반환합니다.
		return nullptr;
	}

	// ParticleEffect를 초기화합니다.
	ParticleEffect->InitializeParticleEffect(ParticleSystem, GetPROwner(), PoolIndex, Lifespan);

	// ParticleEffect의 OnEffectDeactivateDelegate 이벤트에 대한 콜백 함수를 바인딩합니다.
	ParticleEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnParticleEffectDeactivate);

	return ParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::SpawnDynamicParticleEffectInWorld(UParticleSystem* ParticleSystem)
{
	if(!ParticleSystem)
	{
		return nullptr;
	}
	
	APRParticleEffect* DynamicParticleEffect = nullptr;
	
	// Critical Section 시작
	FCriticalSection CriticalSection;
	CriticalSection.Lock();

	// 해당 ParticleSystem의 UsedParticleIndexList가 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateUsedParticleIndexList(ParticleSystem))
	{
		CreateUsedParticleIndexList(ParticleSystem);
	}

	// UsedParticleIndexList에서 해당 ParticleSystem의 UsedIndexList를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedParticleIndexList.List.Find(ParticleSystem);
	if(UsedIndexList == nullptr)
	{
		// 지정된 ParticleSystem이 없습니다.
		return nullptr;
	}

	// 사용 가능한 Index를 구합니다.
	const int32 NewIndex = FindAvailableIndex(UsedIndexList->Indexes);
	// 사용 가능한 Index를 UsedIndexList에 추가합니다.
	UsedIndexList->Indexes.Add(NewIndex);

	// Critical Section 끝
	CriticalSection.Unlock();

	// 새로운 ParticleEffect를 생성하고 초기화합니다.
	const FPRParticleEffectPoolSettings ParticleEffectSettings = GetParticleEffectPoolSettingsFromDataTable(ParticleSystem);
	if(ParticleEffectSettings != FPRParticleEffectPoolSettings())
	{
		// 데이터 테이블에 ParticleEffect의 설정 값을 가지고 있을 경우 설정 값의 Lifespan을 적용합니다.
		DynamicParticleEffect = SpawnParticleEffectInWorld(ParticleSystem, NewIndex, ParticleEffectSettings.EffectLifespan);
	}
	else
	{
		// 데이터 테이블에 ParticleEffect의 설정 값을 가지고 있지 않을 경우 DynamicLifespan을 적용합니다.
		DynamicParticleEffect = SpawnParticleEffectInWorld(ParticleSystem, NewIndex, DynamicLifespan);
	}
		
	// OnDynamicParticleEffectDeactivate 함수를 바인딩합니다.
	DynamicParticleEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicParticleEffectDeactivate);

	// ParticlePool에서 해당 ParticleSystem의 Pool을 얻습니다.
	FPRParticleEffectPool* PoolEntry = ParticlePool.Pool.Find(ParticleSystem);
	if(!PoolEntry)
	{
		// Pool이 없을 경우 생성한 ParticleEffect를 제거하고 nullptr을 반환합니다.
		DynamicParticleEffect->ConditionalBeginDestroy();
		
		return nullptr;
	}

	// 새로 생성한 ParticleEffect를 PoolEntry에 추가합니다.
	PoolEntry->PooledEffects.Emplace(DynamicParticleEffect);

	return DynamicParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::InitializeParticleEffect(UParticleSystem* SpawnEffect)
{
	APRParticleEffect* ActivateableParticleEffect = GetActivateableParticleEffect(SpawnEffect);
	
	// 유효하지 않는 ParticleEffect이거나 풀링 가능한 객체가 아니면 nullptr를 반환합니다.
	if(!IsValid(ActivateableParticleEffect) || !IsPoolableObject(ActivateableParticleEffect))
	{
		return nullptr;
	}

	// 동적으로 생성한 ParticleEffect일 경우 DynamicObjectDestroyTimer가 작동 중이라면 DynamicObjectDestroyTimer를 정지합니다.
	FTimerHandle* DynamicObjectDestroyTimer = DynamicDestroyParticleList.FindTimerHandleForParticleEffect(*ActivateableParticleEffect);
	if(DynamicObjectDestroyTimer)
	{
		GetWorld()->GetTimerManager().ClearTimer(*DynamicObjectDestroyTimer);
	}
	
	// 해당 ParticleSystem를 처음 활성화하는 경우 ActivateParticleEffectIndexList를 생성합니다.
	if(!IsCreateActivateParticleIndexList(SpawnEffect))
	{
		CreateActivateParticleIndexList(SpawnEffect);
	}

	// 활성화된 ParticleEffect의 Index를 ActivateParticleIndexList에 저장합니다.
	const int32 PoolIndex = GetPoolIndex(ActivateableParticleEffect);
	ActivateParticleIndexList.GetIndexesForParticleSystem(*SpawnEffect)->Add(PoolIndex);
	
	return ActivateableParticleEffect;
}

void UPREffectSystemComponent::ClearDynamicDestroyParticleList(FPRDynamicDestroyParticleEffectList& TargetDynamicDestroyParticleEffectList)
{
	ClearDynamicDestroyObjects(TargetDynamicDestroyParticleEffectList.List);
}

FPRParticleEffectPoolSettings UPREffectSystemComponent::GetParticleEffectPoolSettingsFromDataTable(UParticleSystem* ParticleSystem) const
{
	if(ParticlePoolSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = ParticlePoolSettingsDataTable->GetRowNames();
		for(const FName& RowName: RowNames)
		{
			FPRParticleEffectPoolSettings* ParticleEffectPoolSettings = ParticlePoolSettingsDataTable->FindRow<FPRParticleEffectPoolSettings>(RowName, FString(""));
			if(ParticleEffectPoolSettings
				&& ParticleEffectPoolSettings->ParticleSystem == ParticleSystem)
			{
				return *ParticleEffectPoolSettings;
			}
		}
	}

	return FPRParticleEffectPoolSettings();
}

void UPREffectSystemComponent::OnParticleEffectDeactivate(APREffect* TargetEffect)
{
	APRParticleEffect* TargetParticleEffect = Cast<APRParticleEffect>(TargetEffect);
	// 유효하지 않는 ParticleEffect이거나 풀링 가능한 ParticleEffect가 아니거나 ParticleComponent가 없으면 반환합니다.
	if(!IsValid(TargetParticleEffect)
		|| !IsPoolableObject(TargetParticleEffect)
		|| !TargetParticleEffect->GetParticleEffect())
	{
		return;
	}

	// TargetParticleEffect가 활성화된 상태라면 비활성화합니다.
	if(IsActivateParticleEffect(TargetParticleEffect))
	{
		FPRActivateIndexList* ActivateIndexList = ActivateParticleIndexList.List.Find(TargetParticleEffect->GetParticleEffectAsset());
		if(ActivateIndexList)
		{
			// 비활성화된 TargetParticleEffect의 Index를 제거합니다.
			ActivateIndexList->Indexes.Remove(GetPoolIndex(TargetParticleEffect));
		}
	}
}

void UPREffectSystemComponent::OnDynamicParticleEffectDeactivate(APREffect* TargetEffect)
{
	APRParticleEffect* TargetParticleEffect = Cast<APRParticleEffect>(TargetEffect);
	// 유효하지 않는 ParticleEffect이거나 풀링 가능한 ParticleEffect가 아니거나 ParticleComponent가 없으면 반환합니다.
	if(!IsValid(TargetParticleEffect)
		|| !IsPoolableObject(TargetParticleEffect)
		|| !TargetParticleEffect->GetParticleEffect())
	{
		return;
	}

	if(DynamicLifespan > 0.0f)
	{
		// 동적 수명이 끝난 후 ParticleEffect를 제거하도록 타이머를 설정합니다.
		FTimerHandle DynamicLifespanTimerHandle;
		FTimerDelegate DynamicLifespanDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::OnDynamicParticleEffectDestroy, TargetParticleEffect);
		GetWorld()->GetTimerManager().SetTimer(DynamicLifespanTimerHandle, DynamicLifespanDelegate, DynamicLifespan, false);

		// TimerHandle을 추가합니다.
		FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyParticleList.List.Find(TargetParticleEffect->GetParticleEffectAsset());
		if(DynamicDestroyObject)
		{
			DynamicDestroyObject->TimerHandles.Emplace(TargetParticleEffect, DynamicLifespanTimerHandle);
		}
		else
		{
			FPRDynamicDestroyObject NewDynamicDestroyObject;
			NewDynamicDestroyObject.TimerHandles.Emplace(TargetParticleEffect, DynamicLifespanTimerHandle);
			DynamicDestroyParticleList.List.Emplace(TargetParticleEffect->GetParticleEffectAsset(), NewDynamicDestroyObject);
		}
	}
	else
	{
		// 동적 수명이 없을 경우 타이머를 실행하지 않고 바로 ParticleEffect를 제거합니다.
		OnDynamicParticleEffectDestroy(TargetParticleEffect);
	}
}

void UPREffectSystemComponent::OnDynamicParticleEffectDestroy(APRParticleEffect* TargetParticleEffect)
{
	// DynamicObjectDestroyTimer를 제거합니다.
	FPRDynamicDestroyObject* DynamicDestroyObject = DynamicDestroyParticleList.List.Find(TargetParticleEffect->GetParticleEffectAsset());
	if(DynamicDestroyObject)
	{
		FTimerHandle* TimerHandle = DynamicDestroyObject->TimerHandles.Find(TargetParticleEffect);
		GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
		DynamicDestroyObject->TimerHandles.Remove(TargetParticleEffect);
	}

	// ParticleSystem의 UsedObjectIndex를 얻습니다.
	FPRUsedIndexList* UsedIndexList = UsedParticleIndexList.List.Find(TargetParticleEffect->GetParticleEffectAsset());
	if(UsedIndexList)
	{
		// 사용 중인 Index를 제거합니다.
		UsedIndexList->Indexes.Remove(GetPoolIndex(TargetParticleEffect));
	}

	// ParticleSystem의 Pool이 생성되었는지 확인합니다.
	if(IsCreateParticlePool(TargetParticleEffect->GetParticleEffectAsset()))
	{
		// ParticlePool에서 ParticleEffect를 제거합니다.
		FPRParticleEffectPool* PoolEntry = ParticlePool.Pool.Find(TargetParticleEffect->GetParticleEffectAsset());
		if(PoolEntry)
		{
			PoolEntry->PooledEffects.Remove(TargetParticleEffect);
		}
	}
		
	TargetParticleEffect->ConditionalBeginDestroy();
}
#pragma endregion 
