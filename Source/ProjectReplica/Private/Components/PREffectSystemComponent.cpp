// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PREffectSystemComponent.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Effects/PRNiagaraEffect.h"
#include "Effects/PRParticleEffect.h"
#include "Characters/PRBaseCharacter.h"

UPREffectSystemComponent::UPREffectSystemComponent()
{
	// EffectSystem
	DynamicLifespan = 30.0f;
	
	// NiagaraEffect
	NiagaraEffectSettingsDataTable = nullptr;
	NiagaraEffectPool.Empty();
	DynamicNiagaraEffectPool.Empty();
	ActivateNiagaraEffectIndexList.Empty();
	UsedNiagaraEffectIndexList.Empty();
	
	// ParticleEffect
	ParticleEffectPool.Empty();
}

void UPREffectSystemComponent::DestroyComponent(bool bPromoteChildren)
{
	EmptyAllEffectPool();
	
	Super::DestroyComponent(bPromoteChildren);
}

void UPREffectSystemComponent::InitializeEffectPool()
{
	// 기존의 EffectPool을 초기화합니다.
	EmptyAllEffectPool();

	// DataTable의 정보를 기반으로 NiagaraEffectPool을 생성합니다.
	if(NiagaraEffectSettingsDataTable != nullptr)
	{
		TArray<FName> RowNames = NiagaraEffectSettingsDataTable->GetRowNames();
		for(const FName& RowName: RowNames)
		{
			const FPRNiagaraEffectSettings* NiagaraEffectSettings = NiagaraEffectSettingsDataTable->FindRow<FPRNiagaraEffectSettings>(RowName, FString(""));
			if(NiagaraEffectSettings != nullptr)
			{
				CreateNiagaraEffectPool(*NiagaraEffectSettings);
			}
		}
	}
	
	// DataTable의 정보를 기반으로 ParticleEffectPool을 생성합니다.
}

void UPREffectSystemComponent::EmptyAllEffectPool()
{
	EmptyAllNiagaraEffectPool();
	// ParticleEffectPool 비우기
}

int32 UPREffectSystemComponent::FindAvailableIndex(const TSet<int32>& UsedIndexes)
{
	int32 NewIndex = 0;
	while(UsedIndexes.Contains(NewIndex))
	{
		NewIndex++;
	}

	return NewIndex;
}

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

bool UPREffectSystemComponent::IsActivateNiagaraEffect(APRNiagaraEffect* NiagaraEffect) const
{
	if(IsValid(NiagaraEffect))
	{
		const FPRActivateIndexList* ActivateIndexList = ActivateNiagaraEffectIndexList.Find(NiagaraEffect->GetNiagaraEffectAsset());
		if(ActivateIndexList != nullptr)
		{
			return NiagaraEffect->IsActivate() && ActivateIndexList->Indexes.Contains(NiagaraEffect->GetPoolIndex());
		}
	}
	
	return false;
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

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAtLocation(UNiagaraSystem* SpawnEffect, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = GetActivateableNiagaraEffect(SpawnEffect);
	if(IsValid(ActivateableNiagaraEffect))
	{
		// NiagaraEffect를 활성화하고 Spawn할 위치와 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAtLocation(Location, Rotation, Scale, bEffectAutoActivate);

		// 활성화된 NiagaraEffect의 Index를 ActivateNiagaraEffectIndexList에 추가합니다.
		ActivateNiagaraEffectIndexList.Find(SpawnEffect)->Indexes.Add(ActivateableNiagaraEffect->GetPoolIndex());
	}
	
	return ActivateableNiagaraEffect;
}

APRNiagaraEffect* UPREffectSystemComponent::SpawnNiagaraEffectAttached(UNiagaraSystem* SpawnEffect,	USceneComponent* Parent, FName AttachSocketName, FVector Location, FRotator Rotation, FVector Scale, bool bEffectAutoActivate)
{
	APRNiagaraEffect* ActivateableNiagaraEffect = GetActivateableNiagaraEffect(SpawnEffect);
	if(IsValid(ActivateableNiagaraEffect))
	{
		// NiagaraEffect를 활성화하고 Spawn하여 부착할 Component와 위치, 회전값, 크기, 자동실행 여부를 적용합니다.
		ActivateableNiagaraEffect->SpawnEffectAttached(Parent, AttachSocketName, Location, Rotation, Scale, bEffectAutoActivate);

		// 활성화된 NiagaraEffect의 Index를 ActivateNiagaraEffectIndexList에 추가합니다.
		ActivateNiagaraEffectIndexList.Find(SpawnEffect)->Indexes.Add(ActivateableNiagaraEffect->GetPoolIndex());
	}
	
	return ActivateableNiagaraEffect;
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
		FPRUsedIndexList* UsedEffectIndexList = UsedNiagaraEffectIndexList.Find(NiagaraSystem);
		if(UsedEffectIndexList == nullptr)
		{
			// 지정된 NiagaraEffect가 없습니다.
			return nullptr;
		}

		// 사용 가능한 Index를 구합니다.
		const int32 NewIndex = FindAvailableIndex(UsedEffectIndexList->Indexes);

		// 사용 가능한 Index를 UsedEffectIndexList에 추가합니다.
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

		// 새로 생성한 NiagaraEffect를 Pair에 추가합니다.
		Pair->Effects.Emplace(ActivateableNiagaraEffect);
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
		// NiagaraEffectPool에 추가할 Pair를 초기화하고 NiagaraEffect를 생성하여 추가합니다.
		FPRNiagaraEffectPool Pair;
		for(int32 Index = 0; Index < NiagaraEffectSettings.PoolSize; Index++)
		{
			APRNiagaraEffect* SpawnNiagaraEffect = SpawnNiagaraEffectInWorld(NiagaraEffectSettings.NiagaraSystem, Index, NiagaraEffectSettings.Lifespan);
			if(IsValid(SpawnNiagaraEffect))
			{
				Pair.Effects.Emplace(SpawnNiagaraEffect);
			}
		}

		// 초기화된 Pair를 NiagaraEffectPool에 추가하고 ActivateNiagaraEffectIndexList와 UsedNiagaraEffectIndexList를 생성합니다.
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
		UsedEffectIndexList.Indexes.Add(NiagaraEffect->GetPoolIndex());
	}

	UsedNiagaraEffectIndexList.Emplace(NiagaraSystem, UsedEffectIndexList);
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
	// NiagaraEffect->OnNiagaraEffectDeactivate.AddDynamic(this, &UPREffectSystemComponent::OnNiagaraEffectDeactivate);
	NiagaraEffect->OnEffectDeactivateDelegate.AddDynamic(this, &UPREffectSystemComponent::OnNiagaraEffectDeactivate);

	return NiagaraEffect;
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
			ActivateIndexList->Indexes.Remove(NiagaraEffect->GetPoolIndex());
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

			// 타이머를 DynamicDestroyNiagaraEffectList에 추가합니다.
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
		UsedIndexList->Indexes.Remove(NiagaraEffect->GetPoolIndex());
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
