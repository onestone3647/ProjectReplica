// Fill out your copyright notice in the Description page of Project Settings.


#include "Spawners/PRAISpawner.h"
#include "ProjectReplicaGameInstance.h"
#include "Characters/PRBaseCharacter.h"
#include "Components/PRStatSystemComponent.h"
#include "Kismet/GameplayStatics.h"

APRAISpawner::APRAISpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SpawnAIMesh	= CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpawnAIMesh"));
	RootComponent = SpawnAIMesh;

	SpawnAICharacterClass = nullptr;
	SpawnAICharacterLevel = 1;
	SpawnedAICharacter = nullptr;

	// 게임에서 숨깁니다.
	AActor::SetActorHiddenInGame(true);
}

void APRAISpawner::BeginPlay()
{
	Super::BeginPlay();

	if(SpawnAICharacterClass)
	{
		SpawnedAICharacter = GetWorld()->SpawnActor<APRBaseCharacter>(SpawnAICharacterClass);
		SpawnedAICharacter->SetActorLocation(GetActorLocation());
		UProjectReplicaGameInstance* PRGameInstance = Cast<UProjectReplicaGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

		if(IsValid(SpawnedAICharacter) && PRGameInstance)
		{
			SpawnedAICharacter->GetStatSystem()->InitializeStatSystem(SpawnAICharacterLevel, PRGameInstance->GetCharacterStat(SpawnedAICharacter.GetClass(), SpawnAICharacterLevel));
		}
	}
}

#if WITH_EDITOR
void APRAISpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// SpawnAICharacterClass의 값이 변경되었는지 확인합니다.
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(APRAISpawner, SpawnAICharacterClass))
	{
		if(SpawnAICharacterClass)
		{
			ACharacter* AICharacter = SpawnAICharacterClass->GetDefaultObject<ACharacter>();
			if(AICharacter && AICharacter->GetMesh())
			{
				USkeletalMesh* SpawnAISkeletalMesh = AICharacter->GetMesh()->GetSkeletalMeshAsset();
				SpawnAIMesh->SetSkeletalMesh(SpawnAISkeletalMesh);

				// Material을 복사합니다.
				int32 MaterialCount = AICharacter->GetMesh()->GetNumMaterials();
				for (int32 i = 0; i < MaterialCount; i++)
				{
					UMaterialInterface* Material = AICharacter->GetMesh()->GetMaterial(i);
					SpawnAIMesh->SetMaterial(i, Material);
				}
			}
			else
			{
				SpawnAIMesh->SetSkeletalMesh(nullptr);
			}
		}
		else
		{
			SpawnAIMesh->SetSkeletalMesh(nullptr);
		}
	}
}
#endif

