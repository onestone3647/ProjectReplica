// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectReplicaGameMode.generated.h"

class UPRObjectPoolSystemComponent;

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
};



