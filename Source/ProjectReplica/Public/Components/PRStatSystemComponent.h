// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "Components/PRBaseActorComponent.h"
#include "Engine/DataTable.h"
#include "PRStatSystemComponent.generated.h"

/**
 * 캐릭터의 능력치를 관리하는 ActorComponent 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API UPRStatSystemComponent : public UPRBaseActorComponent
{
	GENERATED_BODY()

public:
	UPRStatSystemComponent();

public:
	/** 캐릭터의 능력치를 인자로 받은 레벨에 해당하는 능력치로 초기화하는 함수입니다. */
	UFUNCTION(Blueprintable, Category = "StatSystem")
	void InitializeStatByLevel(int32 NewLevel);
	
	UFUNCTION(BlueprintCallable, Category = "StatSystem")
	void SetHealth(float NewHealth);
	
protected:
	/** 캐릭터의 레벨입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatSystem")
	int32 Level;	
	
	/** 캐릭터의 능력치입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatSystem")
	FPRCharacterStat CharacterStat;

public:
	/** Level을 반환하는 함수입니다. */
	int32 GetLevel() const { return Level; }
	
	/** CharacterStat을 반환하는 함수입니다. */
	FPRCharacterStat GetCharacterStat() const;

	/** 입력받은 인자로 CharacterStat을 설정하는 함수입니다. */
	void SetCharacterStat(const FPRCharacterStat& NewCharacterStat);
};
