// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/PRDamageAmount.h"
#include "Components/WidgetComponent.h"
#include "Widgets/PRDamageAmountWidget.h"

APRDamageAmount::APRDamageAmount()
{
	PrimaryActorTick.bCanEverTick = false;

	// DamageAmountWidgetClass;
	DamageAmountWidgetClass = nullptr;
	static ConstructorHelpers::FClassFinder<UUserWidget> W_PRDamageAmount(TEXT("/Game/Blueprints/Widget/W_PRDamageAmount"));
	if(W_PRDamageAmount.Succeeded() == true)
	{
		DamageAmountWidgetClass = W_PRDamageAmount.Class;
	}

	// DamageAmountWidgetInstance
	DamageAmountWidgetInstance = nullptr;

	// DamageAmountWidget
	DamageAmountWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageAmountWidget"));
	DamageAmountWidget->SetupAttachment(RootComponent);
	DamageAmountWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DamageAmountWidget->SetDrawSize(FVector2D(500.0f, 120.0f));
	if(DamageAmountWidgetClass)
	{
		DamageAmountWidget->SetWidgetClass(DamageAmountWidgetClass);
	}
}

void APRDamageAmount::BeginPlay()
{
	Super::BeginPlay();

	CreateDamageAmountWidget();
}

void APRDamageAmount::Initialize(FVector SpawnLocation, float DamageAmount, bool bIsCritical, EPRElement Element)
{
	SetActorLocation(SpawnLocation);

	if(IsValid(DamageAmountWidgetInstance))
	{
		// WidgetAnimation이 종료되었을 때 OnFadeOutAnimFinish 함수를 실행하기 위해 Delegate에 함수를 바인딩합니다.
		DamageAmountWidgetInstance->OnFadeOutWidgetAnimFinishedDelegate.BindUFunction(this, TEXT("OnFadeOutAnimFinish"));
		
		DamageAmountWidgetInstance->InitializeDamageAmountWidget(DamageAmount, bIsCritical, Element);
	}
}

UPRDamageAmountWidget* APRDamageAmount::CreateDamageAmountWidget()
{
	if(DamageAmountWidgetClass && DamageAmountWidget)
	{
		DamageAmountWidgetInstance = CreateWidget<UPRDamageAmountWidget>(GetWorld(), DamageAmountWidgetClass);
		if(IsValid(DamageAmountWidgetInstance))
		{
			DamageAmountWidget->SetWidget(DamageAmountWidgetInstance);
			
			return DamageAmountWidgetInstance; 
		}
	}

	return nullptr;
}

void APRDamageAmount::OnFadeOutAnimFinish()
{
	Destroy();
}

