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

		FInputModeGameAndUI InputModeGameAndUI;

		SetInputMode(InputModeGameAndUI);
		bShowMouseCursor = true;

	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	if (PauseMenu)
	{
		GameModeOnly();
		bShowMouseCursor = false;

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
}

void AMainPlayerController::DisplayTargetingCrossHair()
{

	if (TargetingCrossHair)
	{
		bTargetingCrossHair = true;
		TargetingCrossHair->SetVisibility(ESlateVisibility::Visible); // ���ü� ����
		UE_LOG(LogTemp, Warning, TEXT("target"))
	}
}

void AMainPlayerController::RemoveTargetingCrossHair()
{
	if (TargetingCrossHair)
	{
		bTargetingCrossHair = false;
		TargetingCrossHair->SetVisibility(ESlateVisibility::Hidden); // ���ü� ����
		UE_LOG(LogTemp, Warning, TEXT("no target!"))
	}
}


void AMainPlayerController::ToggleTargetingCrossHair()
{
	if (bTargetingCrossHair)
	{
		UE_LOG(LogTemp, Warning, TEXT("I Dont See!"))
		RemoveTargetingCrossHair();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("I See!"))
		DisplayTargetingCrossHair();
	}
}
