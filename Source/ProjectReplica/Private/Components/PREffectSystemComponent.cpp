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
	
	// NiagaraEffect를 활성화하고 Spawn할 위치와 회전값, 크기, 자동실행 여부를 적용합니다.
	ActivateableNiagaraEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate, bReset);

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

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect,	USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
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
	
	// NiagaraEffect를 활성화하고 Spawn하여 부착할 Component와 위치, 회전값, 크기, 자동실행 여부를 적용합니다.
	ActivateableNiagaraEffect->SpawnEffectAttached(Parent, AttachSocketName, Location, Rotation, Scale, EAttachLocation::KeepWorldPosition, bEffectAutoActivate, bReset);

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

bool UPREffectSystemComponent::IsActivateNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const
{
	// if(IsValid(NiagaraEffect))
	// {
	// 	const FPRActivateIndexList* ActivateIndexList = ActivateNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset());
	// 	if(ActivateIndexList != nullptr)
	// 	{
	// 		return IPRPoolableInterface::Execute_IsActivate(NiagaraEffect) && ActivateIndexList->Indexes.Contains(IPRPoolableInterface::Execute_GetPoolIndex(NiagaraEffect));
	// 	}
	// }

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

bool UPREffectSystemComponent::IsCreateActivateNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return ActivateNiagaraIndexList.List.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsCreateUsedNiagaraIndexList(UNiagaraSystem* NiagaraSystem) const
{
	return UsedNiagaraIndexList.List.Contains(NiagaraSystem);
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
		FPRNiagaraEffectPool* NiagaraEffectPool = NiagaraPool.Pool.Find(NiagaraSystem);
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

void UPREffectSystemComponent::ClearDynamicDestroyNiagaraList(FPRDynamicDestroyNiagaraEffectList& TargetDynamicDestroyNiagaraEffectList)
{
	ClearDynamicDestroyObjects(TargetDynamicDestroyNiagaraEffectList.List);
}
#pragma endregion 

#pragma region ParticleSystem
void UPREffectSystemComponent::InitializeParticlePool()
{
}

void UPREffectSystemComponent::ClearAllParticlePool()
{
}
#pragma endregion 
































#pragma region NiagaraEffect
void UPREffectSystemComponent::EmptyAllNiagaraEffectPool()
{
	// NiagaraEffectPool 비우기
	if(NiagaraEffectPool.Num() > 0)
	{
		for(auto& Pair : NiagaraEffectPool)
		{
			// 각 항목의 NiagaraEffect에 대한 탐색
			for(auto& NiagaraEffect : Pair.Value.Effects)
			{
				// NiagaraEffect가 유효한지 확인하고, 유효한 경우 제거
				if(IsValid(NiagaraEffect))
				{
					NiagaraEffect->Destroy();
				}
			}

			// NiagaraEffect를 제거한 후 풀의 항목을 비웁니다.
			Pair.Value.Effects.Empty();
		}

		// 모든 항목을 제거한 후 풀을 비웁니다.
		NiagaraEffectPool.Empty();
	}

	// DynamicNiagaraEffectPool 비우기
	if(DynamicNiagaraEffectPool.Num() > 0)
	{
		for(auto& Pair : DynamicNiagaraEffectPool)
		{
			// 각 항목의 동적으로 생성한 NiagaraEffect에 대한 탐색
			for(auto& DynamicNiagaraEffect : Pair.Value.Effects)
			{
				// 동적으로 생성한 NiagaraEffect가 유효한지 확인하고, 유효한 경우 제거
				if(IsValid(DynamicNiagaraEffect.Key))
				{
					DynamicNiagaraEffect.Key->Destroy();
				}

				// TimerHandle이 유효한지 확인하고, 유효한 경우 제거
				if(DynamicNiagaraEffect.Value.IsValid())
				{
					// TimerHandle 제거
					GetWorld()->GetTimerManager().ClearTimer(DynamicNiagaraEffect.Value);
				}
			}

			// 동적으로 생성한 NiagaraEffect를 제거한 후 풀의 항목을 비웁니다.
			Pair.Value.Effects.Empty();
		}

		// 모든 항목을 제거한 후 풀을 비웁니다.
		DynamicNiagaraEffectPool.Empty();
	}

	// ActivateNiagaraEffectIndexList와 UsedNiagaraEffectIndexList 비우기
	ActivateNiagaraEffectIndexList.Empty();
	UsedNiagaraEffectIndexList.Empty();
}

bool UPREffectSystemComponent::IsCreateNiagaraEffectPool(UNiagaraSystem* NiagaraSystem) const
{
	return NiagaraEffectPool.Contains(NiagaraSystem);
}

bool UPREffectSystemComponent::IsDynamicNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const
{
	if(IsValid(NiagaraEffect))
	{
		const FPRDynamicNiagaraEffectPool* DynamicEffectPool = DynamicNiagaraEffectPool.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(DynamicEffectPool != nullptr)
		{
			return DynamicEffectPool->Effects.Contains(NiagaraEffect);
		}
	}
	
	return false;
}

APRNiagaraEffect* UPREffectSystemComponent::GetActivateableNiagaraEffect(UNiagaraSystem* NiagaraSystem)
{
	// 해당 NiagaraEffect에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateNiagaraEffectPool(NiagaraSystem))
	{
		FPRNiagaraEffectSettings NiagaraEffectSettings = FPRNiagaraEffectSettings(NiagaraSystem, 1, DynamicLifespan);
		CreateNiagaraEffectPool(NiagaraEffectSettings);
	}

	// NiagaraEffectPool에서 해당 NiagaraEffect의 Pool을 얻습니다.
	FPRNiagaraEffectPool* Pair = NiagaraEffectPool.Find(NiagaraSystem);
	if(Pair == nullptr)
	{
		// 지정된 NiagaraEffect가 업습니다.
		return nullptr;
	}

	APRNiagaraEffect* ActivateableNiagaraEffect = nullptr;

	// Pair에서 활성화되지 않은 NiagaraEffect를 찾아 활성화합니다.
	for(const auto& NiagaraEffect : Pair->Effects)
	{
		if(IsValid(NiagaraEffect) && !IsActivateNiagaraEffect(NiagaraEffect))
		{
			ActivateableNiagaraEffect = NiagaraEffect;
			break;
		}
	}

	// Pair의 모든 NiagaraEffect가 활성화되었을 경우 새로운 NiagaraEffect를 생성합니다.
	if(ActivateableNiagaraEffect == nullptr)
	{
		ActivateableNiagaraEffect = SpawnDynamicNiagaraEffectInWorld(NiagaraSystem);
	}
	
	// 동적으로 생성된 NiagaraEffect일 경우 DynamicEffectDestroyTimer를 정지합니다.
	FPRDynamicNiagaraEffectPool* DynamicEffectPool = DynamicNiagaraEffectPool.Find(ActivateableNiagaraEffect->GetNiagaraEffectAsset());
	if(DynamicEffectPool != nullptr)
	{
		FTimerHandle* DynamicDestroyTimer = DynamicEffectPool->Effects.Find(ActivateableNiagaraEffect);
		if(DynamicDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicDestroyTimer);			
		}
	}
	
	return ActivateableNiagaraEffect;
}

void UPREffectSystemComponent::CreateNiagaraEffectPool(FPRNiagaraEffectSettings NiagaraEffectSettings)
{
	if(GetWorld() != nullptr && NiagaraEffectSettings.NiagaraSystem != nullptr)
	{
		// NiagaraEffectPool에 저장할 Pair를 초기화하고 NiagaraEffect를 생성하여 저장합니다.
		FPRNiagaraEffectPool Pair;
		for(int32 Index = 0; Index < NiagaraEffectSettings.PoolSize; Index++)
		{
			APRNiagaraEffect* SpawnNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraEffectSettings.NiagaraSystem, Index, NiagaraEffectSettings.Lifespan);
			if(IsValid(SpawnNiagaraEffect))
			{
				Pair.Effects.Emplace(SpawnNiagaraEffect);
			}
		}

		// 초기화된 Pair를 NiagaraEffectPool에 저장하고 ActivateNiagaraEffectIndexList와 UsedNiagaraEffectIndexList를 생성합니다.
		NiagaraEffectPool.Emplace(NiagaraEffectSettings.NiagaraSystem, Pair);
		CreateActivateNiagaraEffectIndexList(NiagaraEffectSettings.NiagaraSystem);
		CreateUsedNiagaraEffectIndexList(NiagaraEffectSettings.NiagaraSystem);
	}
}

void UPREffectSystemComponent::CreateActivateNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem)
{
	ActivateNiagaraEffectIndexList.Emplace(NiagaraSystem);
}

void UPREffectSystemComponent::CreateUsedNiagaraEffectIndexList(UNiagaraSystem* NiagaraSystem)
{
	FPRNiagaraEffectPool* Pair = NiagaraEffectPool.Find(NiagaraSystem);
	if(Pair == nullptr)
	{
		// 지정된 NiagaraEffect가 업습니다.
		return;
	}
	
	FPRUsedIndexList UsedEffectIndexList;
	for(const auto& NiagaraEffect : Pair->Effects)
	{
		UsedEffectIndexList.Indexes.Add(IPRPoolableInterface::Execute_GetPoolIndex(NiagaraEffect));
	}

	UsedNiagaraEffectIndexList.Emplace(NiagaraSystem, UsedEffectIndexList);
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnDynamicNiagaraEffectInWorld(UNiagaraSystem* NiagaraSystem)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = nullptr;

	FPRUsedIndexList* UsedEffectIndexList = UsedNiagaraEffectIndexList.Find(NiagaraSystem);
	if(UsedEffectIndexList == nullptr)
	{
		// 지정된 NiagaraEffect가 없습니다.
		return nullptr;
	}

	// 사용 가능한 Index를 구합니다.
	const int32 NewIndex = FindAvailableIndex(UsedEffectIndexList->Indexes);

	// 사용 가능한 Index를 UsedEffectIndexList에 저장합니다.
	UsedEffectIndexList->Indexes.Add(NewIndex);

	// 새로운 NiagaraEffect를 생성하고 초기화합니다.
	const FPRNiagaraEffectSettings NiagaraEffectSettings = GetNiagaraEffectSettingsFromDataTable(NiagaraSystem);
	if(NiagaraEffectSettings != FPRNiagaraEffectSettings())
	{
		// 데이터 테이블에 NiagaraEffect의 설정 값을 가지고 있을 경우 설정 값의 Lifespan을 적용합니다.
		ActivateableNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraSystem, NewIndex, NiagaraEffectSettings.Lifespan);
	}
	else
	{
		// 데이터 테이블에 NiagaraEffect의 설정 값을 가지고 있지 않을 경우 DynamicLifespan을 적용합니다.
		ActivateableNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraSystem, NewIndex, DynamicLifespan);
	}
		
	if(!IsValid(ActivateableNiagaraEffect))
	{
		// 지정된 NiagaraEffect가 없습니다.
		return nullptr;
	}
		
	// ActivateableNiagaraEffect의 OnNiagaraEffectDeactivate 이벤트에 대한 콜백 함수를 바인딩합니다.
	// 동적으로 생성된 NiagaraEffect에 대한 추가로 비활성화하는 함수입니다.
	ActivateableNiagaraEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate);

	// NiagaraEffectPool에서 해당 NiagaraEffect의 Pool을 얻습니다.
	FPRNiagaraEffectPool* Pair = NiagaraEffectPool.Find(NiagaraSystem);
	if(Pair == nullptr)
	{
		// 지정된 NiagaraEffect가 업습니다.
		return nullptr;
	}
	
	// 새로 생성한 NiagaraEffect를 Pair에 저장합니다.
	Pair->Effects.Emplace(ActivateableNiagaraEffect);

	return ActivateableNiagaraEffect;
}

void UPREffectSystemComponent::OnNiagaraEffectDeactivate(APREffect* Effect)
{
	APRNiagaraEffect* NiagaraEffect = Cast<APRNiagaraEffect>(Effect);
	if(IsValid(NiagaraEffect) && NiagaraEffect->GetNiagaraEffect() != nullptr)
	{
		FPRActivateIndexList* ActivateIndexList = ActivateNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(ActivateIndexList != nullptr)
		{
			// 비활성화된 NiagaraEffect의 Index를 ActivateIndexList에서 제거합니다.
			ActivateIndexList->Indexes.Remove(IPRPoolableInterface::Execute_GetPoolIndex(NiagaraEffect));
		}
	}
}

void UPREffectSystemComponent::OnDynamicNiagaraEffectDeactivate(APREffect* Effect)
{
	APRNiagaraEffect* NiagaraEffect = Cast<APRNiagaraEffect>(Effect);
	if(IsValid(NiagaraEffect))
	{
		if(DynamicLifespan > 0.0f)
		{
			// 타이머로 동적 수명이 끝난 후 DynamicNiagaraEffectDestroy 함수를 실행합니다.
			FTimerHandle DynamicEffectDestroyTimerHandle;
			FTimerDelegate DynamicEffectDestroyDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::DynamicNiagaraEffectDestroy, NiagaraEffect);
			GetWorld()->GetTimerManager().SetTimer(DynamicEffectDestroyTimerHandle, DynamicEffectDestroyDelegate, DynamicLifespan, false);

			// 타이머를 DynamicDestroyNiagaraEffectList에 저장합니다.
			FPRDynamicNiagaraEffectPool* DynamicPair = DynamicNiagaraEffectPool.Find(NiagaraEffect->GetNiagaraEffectAsset());
			if(DynamicPair != nullptr)
			{
				DynamicPair->Effects.Emplace(NiagaraEffect, DynamicEffectDestroyTimerHandle);
			}
			else
			{
				FPRDynamicNiagaraEffectPool DynamicEffectPool;
				DynamicEffectPool.Effects.Emplace(NiagaraEffect, DynamicEffectDestroyTimerHandle);
				DynamicNiagaraEffectPool.Emplace(NiagaraEffect->GetNiagaraEffectAsset(), DynamicEffectPool);
			}
		}
		else
		{
			// 동적 수명이 없을 경우 타이머를 실행하지 않고 바로 NiagaraEffect를 제거합니다.
			DynamicNiagaraEffectDestroy(NiagaraEffect);
		}
	}
}

void UPREffectSystemComponent::DynamicNiagaraEffectDestroy(APRNiagaraEffect* NiagaraEffect)
{
	// DynamicNiagaraEffectPool에서 해당 NiagaraEffect를 제거합니다.
	FPRDynamicNiagaraEffectPool* DynamicEffectPool = DynamicNiagaraEffectPool.Find(NiagaraEffect->GetNiagaraEffectAsset());
	if(DynamicEffectPool != nullptr)
	{
		DynamicEffectPool->Effects.Remove(NiagaraEffect);
	}

	// UsedNiagaraEffectIndexList에서 해당 NiagaraEffect의 PoolIndex를 제거합니다.
	FPRUsedIndexList* UsedIndexList = UsedNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset());
	if(UsedIndexList != nullptr)
	{
		UsedIndexList->Indexes.Remove(IPRPoolableInterface::Execute_GetPoolIndex(NiagaraEffect));
	}

	// NiagaraEffectPool에서 해당 NiagaraEffect를 제거하고, 월드에서 제거합니다.
	FPRNiagaraEffectPool* EffectPool = NiagaraEffectPool.Find(NiagaraEffect->GetNiagaraEffectAsset());
	if(EffectPool != nullptr)
	{
		EffectPool->Effects.Remove(NiagaraEffect);
		NiagaraEffect->Destroy();
	}
}

FPRNiagaraEffectSettings UPREffectSystemComponent::GetNiagaraEffectSettingsFromDataTable(UNiagaraSystem* NiagaraSystem) const
{
	if(NiagaraEffectSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = NiagaraEffectSettingsDataTable->GetRowNames();
		for(const FName& RowName: RowNames)
		{
			FPRNiagaraEffectSettings* NiagaraEffectSettings = NiagaraEffectSettingsDataTable->FindRow<FPRNiagaraEffectSettings>(RowName, FString(""));
			if(NiagaraEffectSettings != nullptr && NiagaraEffectSettings->NiagaraSystem == NiagaraSystem)
			{
				return *NiagaraEffectSettings;
			}
		}
	}

	return FPRNiagaraEffectSettings();
}
#pragma endregion 

#pragma region ParticleEffect
void UPREffectSystemComponent::EmptyAllParticleEffectPool()
{
	// ParticleEffectPool 비우기
	if(ParticleEffectPool.Num() > 0)
	{
		for(auto& Pair : ParticleEffectPool)
		{
			// 각 항목의 ParticleEffect에 대한 탐색
			for(auto& ParticleEffect : Pair.Value.Effects)
			{
				// ParticleEffect가 유효한지 확인하고, 유효한 경우 제거
				if(IsValid(ParticleEffect))
				{
					ParticleEffect->Destroy();
				}
			}

			// ParticleEffect를 제거한 후 풀의 항목을 비웁니다.
			Pair.Value.Effects.Empty();
		}

		// 모든 항목을 제거한 후 풀을 비웁니다.
		ParticleEffectPool.Empty();
	}

	// DynamicParticleEffectPool 비우기
	if(DynamicParticleEffectPool.Num() > 0)
	{
		for(auto& Pair : DynamicParticleEffectPool)
		{
			// 각 항목의 동적으로 생성한 ParticleEffect에 대한 탐색
			for(auto& DynamicParticleEffect : Pair.Value.Effects)
			{
				// 동적으로 생성한 ParticleEffect가 유효한지 확인하고, 유효한 경우 제거
				if(IsValid(DynamicParticleEffect.Key))
				{
					DynamicParticleEffect.Key->Destroy();
				}

				// TimerHandle이 유효한지 확인하고, 유효한 경우 제거
				if(DynamicParticleEffect.Value.IsValid())
				{
					// TimerHandle 제거
					GetWorld()->GetTimerManager().ClearTimer(DynamicParticleEffect.Value);
				}
			}

			// 동적으로 생성한 ParticleEffect를 제거한 후 풀의 항목을 비웁니다.
			Pair.Value.Effects.Empty();
		}

		// 모든 항목을 제거한 후 풀을 비웁니다.
		DynamicParticleEffectPool.Empty();
	}

	// ActivateParticleEffectIndexList와 UsedParticleEffectIndexList 비우기
	ActivateParticleEffectIndexList.Empty();
	UsedParticleEffectIndexList.Empty();
}

bool UPREffectSystemComponent::IsActivateParticleEffect(APRParticleEffect* ParticleEffect) const
{
	if(IsValid(ParticleEffect))
	{
		const FPRActivateIndexList* ActivateIndexList = ActivateParticleEffectIndexList.Find(ParticleEffect->GetParticleEffectAsset());
		if(ActivateIndexList != nullptr)
		{
			return IPRPoolableInterface::Execute_IsActivate(ParticleEffect) && ActivateIndexList->Indexes.Contains(IPRPoolableInterface::Execute_GetPoolIndex(ParticleEffect));
		}
	}
	
	return false;
}

bool UPREffectSystemComponent::IsCreateParticleEffectPool(UParticleSystem* ParticleSystem) const
{
	return ParticleEffectPool.Contains(ParticleSystem);
}

bool UPREffectSystemComponent::IsDynamicParticleEffect(APRParticleEffect* ParticleEffect) const
{
	if(IsValid(ParticleEffect))
	{
		const FPRDynamicParticleEffectPool* DynamicEffectPool = DynamicParticleEffectPool.Find(ParticleEffect->GetParticleEffectAsset());
		if(DynamicEffectPool != nullptr)
		{
			return DynamicEffectPool->Effects.Contains(ParticleEffect);
		}
	}
	
	return false;
}

APRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAtLocation(UParticleSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate, bool bReset)
{
	APRParticleEffect* ActivateableParticleEffect = GetActivateableParticleEffect(SpawnEffect);
	if(IsValid(ActivateableParticleEffect))
	{
		// ParticleEffect를 활성화하고 Spawn할 위치와 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableParticleEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate, bReset);

		// 활성화된 ParticleEffect의 Index를 ActivateParticleEffectIndexList에 저장합니다.
		ActivateParticleEffectIndexList.Find(SpawnEffect)->Indexes.Add(IPRPoolableInterface::Execute_GetPoolIndex(ActivateableParticleEffect));
	}
	
	return ActivateableParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::SpawnParticleEffectAttached(UParticleSystem* SpawnEffect, USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale,	bool bEffectAutoActivate, bool bReset)
{
	APRParticleEffect* ActivateableParticleEffect = GetActivateableParticleEffect(SpawnEffect);
	if(IsValid(ActivateableParticleEffect))
	{
		// ParticleEffect를 활성화하고 Spawn하여 부착할 Component와 위치, 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableParticleEffect->SpawnEffectAttached(Parent, AttachSocketName, Location, Rotation, Scale, EAttachLocation::KeepWorldPosition, bEffectAutoActivate, bReset);

		// 활성화된 ParticleEffect의 Index를 ActivateParticleEffectIndexList에 저장합니다.
		ActivateParticleEffectIndexList.Find(SpawnEffect)->Indexes.Add(IPRPoolableInterface::Execute_GetPoolIndex(ActivateableParticleEffect));
	}
	
	return ActivateableParticleEffect;
}

APRParticleEffect* UPREffectSystemComponent::GetActivateableParticleEffect(UParticleSystem* ParticleSystem)
{
	// 해당 ParticleEffect에 해당하는 Pool이 생성되었는지 확인하고, 없으면 생성합니다.
	if(!IsCreateParticleEffectPool(ParticleSystem))
	{
		FPRParticleEffectSettings ParticleEffectSettings = FPRParticleEffectSettings(ParticleSystem, 1, DynamicLifespan);
		CreateParticleEffectPool(ParticleEffectSettings);
	}

	// ParticleEffectPool에서 해당 ParticleEffect의 Pool을 얻습니다.
	FPRParticleEffectPool* Pair = ParticleEffectPool.Find(ParticleSystem);
	if(Pair == nullptr)
	{
		// 지정된 ParticleEffect가 업습니다.
		return nullptr;
	}

	APRParticleEffect* ActivateableParticleEffect = nullptr;

	// Pair에서 활성화되지 않은 ParticleEffect를 찾아 활성화합니다.
	for(const auto& ParticleEffect : Pair->Effects)
	{
		if(IsValid(ParticleEffect) && !IsActivateParticleEffect(ParticleEffect))
		{
			ActivateableParticleEffect = ParticleEffect;
			break;
		}
	}

	// Pair의 모든 ParticleEffect가 활성화되었을 경우 새로운 ParticleEffect를 생성합니다.
	if(ActivateableParticleEffect == nullptr)
	{
		ActivateableParticleEffect = SpawnDynamicParticleEffectInWorld(ParticleSystem);
	}
	
	// 동적으로 생성된 ParticleEffect일 경우 DynamicEffectDestroyTimer를 정지합니다.
	FPRDynamicParticleEffectPool* DynamicEffectPool = DynamicParticleEffectPool.Find(ActivateableParticleEffect->GetParticleEffectAsset());
	if(DynamicEffectPool != nullptr)
	{
		FTimerHandle* DynamicDestroyTimer = DynamicEffectPool->Effects.Find(ActivateableParticleEffect);
		if(DynamicDestroyTimer != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*DynamicDestroyTimer);			
		}
	}
	
	return ActivateableParticleEffect;
}

void UPREffectSystemComponent::CreateParticleEffectPool(FPRParticleEffectSettings ParticleEffectSettings)
{
	if(GetWorld() != nullptr && ParticleEffectSettings.ParticleSystem != nullptr)
	{
		// ParticleEffectPool에 저장할 Pair를 초기화하고 ParticleEffect를 생성하여 저장합니다.
		FPRParticleEffectPool Pair;
		for(int32 Index = 0; Index < ParticleEffectSettings.PoolSize; Index++)
		{
			APRParticleEffect* SpawnParticleEffect = SpawnParticleEffectInWorld(ParticleEffectSettings.ParticleSystem, Index, ParticleEffectSettings.Lifespan);
			if(IsValid(SpawnParticleEffect))
			{
				Pair.Effects.Emplace(SpawnParticleEffect);
			}
		}

		// 초기화된 Pair를 ParticleEffectPool에 저장하고 ActivateParticleEffectIndexList와 UsedParticleEffectIndexList를 생성합니다.
		ParticleEffectPool.Emplace(ParticleEffectSettings.ParticleSystem, Pair);
		CreateActivateParticleEffectIndexList(ParticleEffectSettings.ParticleSystem);
		CreateUsedParticleEffectIndexList(ParticleEffectSettings.ParticleSystem);
	}
}

void UPREffectSystemComponent::CreateActivateParticleEffectIndexList(UParticleSystem* ParticleSystem)
{
	ActivateParticleEffectIndexList.Emplace(ParticleSystem);
}

void UPREffectSystemComponent::CreateUsedParticleEffectIndexList(UParticleSystem* ParticleSystem)
{
	FPRParticleEffectPool* Pair = ParticleEffectPool.Find(ParticleSystem);
	if(Pair == nullptr)
	{
		// 지정된 ParticleEffect가 업습니다.
		return;
	}
	
	FPRUsedIndexList UsedEffectIndexList;
	for(const auto& ParticleEffect : Pair->Effects)
	{
		UsedEffectIndexList.Indexes.Add(IPRPoolableInterface::Execute_GetPoolIndex(ParticleEffect));
	}

	UsedParticleEffectIndexList.Emplace(ParticleSystem, UsedEffectIndexList);
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
	APRParticleEffect* ActivateableParticleEffect = nullptr;
	
	FPRUsedIndexList* UsedEffectIndexList = UsedParticleEffectIndexList.Find(ParticleSystem);
	if(UsedEffectIndexList == nullptr)
	{
		// 지정된 ParticleEffect가 없습니다.
		return nullptr;
	}

	// 사용 가능한 Index를 구합니다.
	const int32 NewIndex = FindAvailableIndex(UsedEffectIndexList->Indexes);

	// 사용 가능한 Index를 UsedEffectIndexList에 저장합니다.
	UsedEffectIndexList->Indexes.Add(NewIndex);

	// 새로운 ParticleEffect를 생성하고 초기화합니다.
	const FPRParticleEffectSettings ParticleEffectSettings = GetParticleEffectSettingsFromDataTable(ParticleSystem);
	if(ParticleEffectSettings != FPRParticleEffectSettings())
	{
		// 데이터 테이블에 ParticleEffect의 설정 값을 가지고 있을 경우 설정 값의 Lifespan을 적용합니다.
		ActivateableParticleEffect = SpawnParticleEffectInWorld(ParticleSystem, NewIndex, ParticleEffectSettings.Lifespan);
	}
	else
	{
		// 데이터 테이블에 ParticleEffect의 설정 값을 가지고 있지 않을 경우 DynamicLifespan을 적용합니다.
		ActivateableParticleEffect = SpawnParticleEffectInWorld(ParticleSystem, NewIndex, DynamicLifespan);
	}
		
	if(!IsValid(ActivateableParticleEffect))
	{
		// 지정된 ParticleEffect가 없습니다.
		return nullptr;
	}
		
	// ActivateableParticleEffect의 OnParticleEffectDeactivate 이벤트에 대한 콜백 함수를 바인딩합니다.
	// 동적으로 생성된 ParticleEffect에 대한 추가로 비활성화하는 함수입니다.
	ActivateableParticleEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnDynamicParticleEffectDeactivate);

	// ParticleEffectPool에서 해당 ParticleEffect의 Pool을 얻습니다.
	FPRParticleEffectPool* Pair = ParticleEffectPool.Find(ParticleSystem);
	if(Pair == nullptr)
	{
		// 지정된 ParticleEffect가 업습니다.
		return nullptr;
	}
	
	// 새로 생성한 ParticleEffect를 Pair에 저장합니다.
	Pair->Effects.Emplace(ActivateableParticleEffect);

	return ActivateableParticleEffect;
}

void UPREffectSystemComponent::OnParticleEffectDeactivate(APREffect* Effect)
{
	APRParticleEffect* ParticleEffect = Cast<APRParticleEffect>(Effect);
	if(IsValid(ParticleEffect) && ParticleEffect->GetParticleEffect() != nullptr)
	{
		FPRActivateIndexList* ActivateIndexList = ActivateParticleEffectIndexList.Find(ParticleEffect->GetParticleEffectAsset());
		if(ActivateIndexList != nullptr)
		{
			// 비활성화된 ParticleEffect의 Index를 ActivateIndexList에서 제거합니다.
			ActivateIndexList->Indexes.Remove(IPRPoolableInterface::Execute_GetPoolIndex(ParticleEffect));
		}
	}
}

void UPREffectSystemComponent::OnDynamicParticleEffectDeactivate(APREffect* Effect)
{
	APRParticleEffect* ParticleEffect = Cast<APRParticleEffect>(Effect);
	if(IsValid(ParticleEffect))
	{
		if(DynamicLifespan > 0.0f)
		{
			// 타이머로 동적 수명이 끝난 후 DynamicParticleEffectDestroy 함수를 실행합니다.
			FTimerHandle DynamicEffectDestroyTimerHandle;
			FTimerDelegate DynamicEffectDestroyDelegate = FTimerDelegate::CreateUObject(this, &UPREffectSystemComponent::DynamicParticleEffectDestroy, ParticleEffect);
			GetWorld()->GetTimerManager().SetTimer(DynamicEffectDestroyTimerHandle, DynamicEffectDestroyDelegate, DynamicLifespan, false);

			// 타이머를 DynamicDestroyParticleEffectList에 저장합니다.
			FPRDynamicParticleEffectPool* DynamicPair = DynamicParticleEffectPool.Find(ParticleEffect->GetParticleEffectAsset());
			if(DynamicPair != nullptr)
			{
				DynamicPair->Effects.Emplace(ParticleEffect, DynamicEffectDestroyTimerHandle);
			}
			else
			{
				FPRDynamicParticleEffectPool DynamicEffectPool;
				DynamicEffectPool.Effects.Emplace(ParticleEffect, DynamicEffectDestroyTimerHandle);
				DynamicParticleEffectPool.Emplace(ParticleEffect->GetParticleEffectAsset(), DynamicEffectPool);
			}
		}
		else
		{
			// 동적 수명이 없을 경우 타이머를 실행하지 않고 바로 ParticleEffect를 제거합니다.
			DynamicParticleEffectDestroy(ParticleEffect);
		}
	}
}

void UPREffectSystemComponent::DynamicParticleEffectDestroy(APRParticleEffect* ParticleEffect)
{
	// DynamicParticleEffectPool에서 해당 ParticleEffect를 제거합니다.
	FPRDynamicParticleEffectPool* DynamicEffectPool = DynamicParticleEffectPool.Find(ParticleEffect->GetParticleEffectAsset());
	if(DynamicEffectPool != nullptr)
	{
		DynamicEffectPool->Effects.Remove(ParticleEffect);
	}

	// UsedParticleEffectIndexList에서 해당 ParticleEffect의 PoolIndex를 제거합니다.
	FPRUsedIndexList* UsedIndexList = UsedParticleEffectIndexList.Find(ParticleEffect->GetParticleEffectAsset());
	if(UsedIndexList != nullptr)
	{
		UsedIndexList->Indexes.Remove(IPRPoolableInterface::Execute_GetPoolIndex(ParticleEffect));
	}

	// ParticleEffectPool에서 해당 ParticleEffect를 제거하고, 월드에서 제거합니다.
	FPRParticleEffectPool* EffectPool = ParticleEffectPool.Find(ParticleEffect->GetParticleEffectAsset());
	if(EffectPool != nullptr)
	{
		EffectPool->Effects.Remove(ParticleEffect);
		ParticleEffect->Destroy();
	}
}

FPRParticleEffectSettings UPREffectSystemComponent::GetParticleEffectSettingsFromDataTable(UParticleSystem* ParticleSystem) const
{
	if(ParticleEffectSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = ParticleEffectSettingsDataTable->GetRowNames();
		for(const FName& RowName: RowNames)
		{
			FPRParticleEffectSettings* ParticleEffectSettings = ParticleEffectSettingsDataTable->FindRow<FPRParticleEffectSettings>(RowName, FString(""));
			if(ParticleEffectSettings != nullptr && ParticleEffectSettings->ParticleSystem == ParticleSystem)
			{
				return *ParticleEffectSettings;
			}
		}
	}

	return FPRParticleEffectSettings();
}
#pragma endregion 
