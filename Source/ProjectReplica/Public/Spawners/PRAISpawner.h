// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRAISpawner.generated.h"

class APRAICharacter;

/**
 * AI를 현재 위치에 Spawn하는 Actor 클래스입니다. 
 */
UCLASS()
class PROJECTREPLICA_API APRAISpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	APRAISpawner();

protected:
	virtual void BeginPlay() override;

	/** SpawnAICharacterClass에 해당하는 AI 캐릭터를 월드에 Spawn하는 함수입니다. */
	void SpawnAI();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawner")
	TObjectPtr<USkeletalMeshComponent> SpawnAIMesh;

	/** Spawn할 AI 캐릭터의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	TSubclassOf<APRBaseCharacter> SpawnAICharacterClass;

	/** Spawn할 AI 캐릭터의 레벨입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	int32 SpawnAICharacterLevel;

	/** Spawn된 AI 캐릭터입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawner")
	TObjectPtr<APRAICharacter> SpawnedAICharacter;
};
