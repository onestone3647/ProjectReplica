// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PRPoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPRPoolableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 오브젝트 풀에 관련된 Interface 클래스입니다.
 */
class PROJECTREPLICA_API IPRPoolableInterface
{
	GENERATED_BODY()

public:
	/**
	 * 오브젝트를 초기화하는 함수입니다.
	 * 
	 * @param NewOwner 오브젝트의 소유자
	 * @param NewPoolIndex 오브젝트 풀의 Index
	 * @param NewLifespan 오브젝트의 수명
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void Initialize(AActor* NewOwner = nullptr, int32 NewPoolIndex = -1, float NewLifespan);

	/** 오브젝트가 활성화 되었는지 판별하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	bool IsActivate() const;
	
	/** 오브젝트를 활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void Activate();

	/** 오브젝트를 비활성화하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	void Deactivate();

	/** 오브젝트의 PoolIndex를 반환하는 함수입니다. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Poolable")
	int32 GetPoolIndex();
};
