// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ProjectReplica.h"
#include "GameFramework/Actor.h"
#include "PRAISpawner.generated.h"

class APRBaseCharacter;
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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AISpawner")
	TObjectPtr<USkeletalMeshComponent> SpawnAIMesh;

	/** Spawn할 AI 캐릭터의 클래스입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AISpawner")
	TSubclassOf<ACharacter> SpawnAICharacterClass;
};
