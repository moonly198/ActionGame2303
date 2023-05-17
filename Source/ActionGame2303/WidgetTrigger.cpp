// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetTrigger.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Main.h"
#include "Blueprint/UserWidget.h"
#include "MainPlayerController.h"

// Sets default values
AWidgetTrigger::AWidgetTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateAbstractDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetBoxExtent(FVector(90.f, 20.f, 20.f));

	widgetStatus = 0;

}

// Called when the game starts or when spawned
void AWidgetTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (widgetStatus == 1)
	{
		WidgetTriggerStatus = EWidgetTriggerStatus::WTS_Time;
	}
	else if (widgetStatus == 2)
	{
		WidgetTriggerStatus = EWidgetTriggerStatus::WTS_Press;
	}


	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWidgetTrigger::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AWidgetTrigger::OnOverlapEnd);
	
}

// Called every frame
void AWidgetTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWidgetTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMain* Main = Cast<AMain>(OtherActor);
	AMainPlayerController* MPC = Cast<AMainPlayerController>(Main->GetController());

	

	if (Main)
	{
		if (WidgetAsset)
		{

			Widget = CreateWidget<UUserWidget>(MPC, WidgetAsset);
			if (Widget)
			{
				Widget->AddToViewport();
				Widget->SetVisibility(ESlateVisibility::Hidden);
				//WidgetList.AddUnique(Widget);
			}
		}

		if (Widget)
		{
			if (WidgetTriggerStatus == EWidgetTriggerStatus::WTS_Time)
			{
				FTimerHandle WaitHandle1;

				Widget->SetVisibility(ESlateVisibility::Visible);

				GetWorld()->GetTimerManager().SetTimer(WaitHandle1, FTimerDelegate::CreateLambda([&]()
					{

						Widget->SetVisibility(ESlateVisibility::Hidden);
						Widget->RemoveFromParent();
					}), HiddenWaitTime, false);
			}
			else if (WidgetTriggerStatus == EWidgetTriggerStatus::WTS_Press)
			{
				MPC->GameAndUI();
				Widget->SetVisibility(ESlateVisibility::Visible);
			}
			



		}
	}


}
void AWidgetTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	Destroy();
}

void AWidgetTrigger::SetWidgetTriggerStatus(EWidgetTriggerStatus Status)
{
	WidgetTriggerStatus = Status;
}

EWidgetTriggerStatus AWidgetTrigger::GetWidgetTriggerStatus()
{
	return WidgetTriggerStatus;
}
