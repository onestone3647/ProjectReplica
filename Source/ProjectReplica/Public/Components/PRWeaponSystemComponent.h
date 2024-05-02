// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "PRWeaponSystemComponent.generated.h"

/**
 * 캐릭터가 사용하는 무기를 관리하는 ActorComponent 클래스입니다.
 */
UCLASS()
class PROJECTREPLICA_API UPRWeaponSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRWeaponSystemComponent();
};
