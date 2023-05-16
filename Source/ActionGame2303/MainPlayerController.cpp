// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();


	

	if (TargetingCrossHairAsset)
	{
		TargetingCrossHair = CreateWidget<UUserWidget>(this, TargetingCrossHairAsset);
		if (TargetingCrossHair)
		{
			TargetingCrossHair->AddToViewport();
			TargetingCrossHair->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����
		}
		FVector2D Alignment(0.f, 0.f);
		TargetingCrossHair->SetAlignmentInViewport(Alignment);
	}


	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible); // ���ü� ����
		}
	}

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

		//FVector2D Alignment(0.f, 0.f);
		//EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	//��Ʈ���� �߰��ϱ�
	if (Hint1Asset)
	{
		Hint1 = CreateWidget<UUserWidget>(this, Hint1Asset);
		if (Hint1)
		{
			Hint1->AddToViewport();
			Hint1->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Hint2Asset)
	{
		Hint2 = CreateWidget<UUserWidget>(this, Hint2Asset);
		if (Hint2)
		{
			Hint2->AddToViewport();
			Hint2->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Hint3Asset)
	{
		Hint3 = CreateWidget<UUserWidget>(this, Hint3Asset);
		if (Hint3)
		{
			Hint3->AddToViewport();
			Hint3->SetVisibility(ESlateVisibility::Hidden);
		}
	}


	if (WidgetTriggerAsset)
	{
		WidgetTrigger = CreateWidget<UUserWidget>(this, WidgetTriggerAsset);
		if (WidgetTrigger)
		{
			WidgetTrigger->AddToViewport();
			WidgetTrigger->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����
			WidgetTriggerList.Add(WidgetTrigger);
		}
	}


	
	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);
		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����

		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetingCrossHair)
	{
		
		FVector2D PositionInViewport;
		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport); // ȭ�鿡�� vector ��� ���
		PositionInViewport.Y -= 0.f;
		PositionInViewport.X -= 0.f;
		
		//FVector2D SizeInViewPort = FVector2D(500, 500);

		TargetingCrossHair->SetPositionInViewport(PositionInViewport);
		//TargetingCrossHair->SetDesiredSizeInViewport(SizeInViewPort);
	}

}

void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}


void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	if (PauseMenu)
	{
		bPauseMenuVisible = true;
		PauseMenu->SetVisibility(ESlateVisibility::Visible); // ���ü� ����

		GameAndUI();
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();

		bPauseMenuVisible = false;
		PauseMenu->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����

	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}

void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;

	SetInputMode(InputModeGameOnly);
	bShowMouseCursor = false;
	UE_LOG(LogTemp, Warning, TEXT("GameModeOnly"));
}

void AMainPlayerController::GameAndUI()
{

	FInputModeGameAndUI InputModeGameAndUI;

	SetInputMode(InputModeGameAndUI);
	bShowMouseCursor = true;
	UE_LOG(LogTemp, Warning, TEXT("GameAndUI"));
}

void AMainPlayerController::DisplayTargetingCrossHair()
{

	if (TargetingCrossHair)
	{
		bTargetingCrossHair = true;
		TargetingCrossHair->SetVisibility(ESlateVisibility::Visible); // ���ü� ����
	}
}

void AMainPlayerController::RemoveTargetingCrossHair()
{
	if (TargetingCrossHair)
	{
		bTargetingCrossHair = false;
		TargetingCrossHair->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����
	}
}


void AMainPlayerController::ToggleTargetingCrossHair()
{
	if (bTargetingCrossHair)
	{
		RemoveTargetingCrossHair();
	}
	else
	{
		DisplayTargetingCrossHair();
	}
}
