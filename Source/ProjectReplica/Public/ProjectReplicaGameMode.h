// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/GameModeBase.h"
#include "Objects/PRPooledObject.h"
#include "ProjectReplicaGameMode.generated.h"

class UPRObjectPoolSystemComponent;
class APRDamageAmount;

UCLASS(minimalapi)
class AProjectReplicaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectReplicaGameMode();

protected:
	virtual void PostInitializeComponents() override;

#pragma region ObjectPoolSystem
	
private:
	/** 오브젝트 풀을 관리하는 ActorComponent 클래스입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectPoolSystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPRObjectPoolSystemComponent> ObjectPoolSystem;

public:
	/** ObjectPoolSystem을 반환하는 함수입니다. */
	FORCEINLINE class UPRObjectPoolSystemComponent* GetObjectPoolSystem() const { return ObjectPoolSystem; }
#pragma endregion

#pragma region DamageAmount
public:
	/**
	 * 초기화한 DamageAmount를 활성화하고, 반환하는 함수입니다.
	 * 
	 * @param SpawnLocation Spawn할 위치
	 * @param DamageAmount 대미지의 양
	 * @param bIsCritical 일반 대미지인지, 치명타 대미지인지 판별하는 인자
	 * @param Element 대미지의 속성
	 * @return 활성화한 DamageAmount
	 */
	UFUNCTION(BlueprintCallable, Category = "DamageAmount")
	class APRDamageAmount* ActivateDamageAmount(FVector SpawnLocation, float DamageAmount, bool bIsCritical, EPRElement Element);

private:
	/** DamageAmount의 클래스 레퍼런스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DamageAmount", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APRPooledObject> DamageAmountClass;
#pragma endregion 
};



