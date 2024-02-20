// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PRBaseActorComponent.h"
#include "ProjectReplicaGameInstance.h"
#include "ProjectReplicaGameMode.h"
#include "Characters/PRBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

UPRBaseActorComponent::UPRBaseActorComponent()
{
	// 기본적으로 Tick 함수를 사용하지 않습니다.
	// Tick 함수를 사용할 클래스에서 true로 설정합니다.
	PrimaryComponentTick.bCanEverTick = false;

	// PRCharacterReference
	PROwner = nullptr;
}

void UPRBaseActorComponent::OnRegister()
{
	Super::OnRegister();

	// PROwner를 초기화합니다.
	InitializePROwner();
}

void UPRBaseActorComponent::InitializePROwner()
{
	APRBaseCharacter* NewPROwner = Cast<APRBaseCharacter>(GetOwner());
	if(IsValid(NewPROwner))
	{
		PROwner = NewPROwner;
	}
}

UProjectReplicaGameInstance* UPRBaseActorComponent::GetPRGameInstance() const
{
	if(GetOwner() && GetOwner()->GetWorld())
	{
		UProjectReplicaGameInstance* PRGameInstance = Cast<UProjectReplicaGameInstance>(GetOwner()->GetWorld()->GetGameInstance());
		if(PRGameInstance != nullptr)
		{
			return PRGameInstance;
		}
	}

	return nullptr;
}

AProjectReplicaGameMode* UPRBaseActorComponent::GetPRGameMode() const
{
	if(GetOwner() && GetOwner()->GetWorld())
	{
		AProjectReplicaGameMode* PRGameMode = Cast<AProjectReplicaGameMode>(UGameplayStatics::GetGameMode(GetWorld()));	
		if(PRGameMode != nullptr)
		{
			return PRGameMode;
		}
	}

	return nullptr;
}

APRBaseCharacter* UPRBaseActorComponent::GetPROwner() const
{
	return PROwner.Get();
}
