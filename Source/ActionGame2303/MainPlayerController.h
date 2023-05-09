// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME2303_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		TSubclassOf<class UUserWidget> TargetingCrossHairAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		UUserWidget* TargetingCrossHair;


	//�����Ϳ��� UMG ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		TSubclassOf <class UUserWidget> HUDOverlayAsset;

	//������ ������ �� ������ ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		UUserWidget* HUDOverlay;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		TSubclassOf<class UUserWidget>WEnemyHealthBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		UUserWidget* EnemyHealthBar;

	bool bEnemyHealthBarVisible;

	void DisplayEnemyHealthBar();
	void RemoveEnemyHealthBar();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		TSubclassOf<class UUserWidget>WPauseMenu;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		UUserWidget* PauseMenu;

	bool bPauseMenuVisible;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
		void DisplayPauseMenu();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD")
		void RemovePauseMenu();

	void TogglePauseMenu();

	void GameModeOnly();

	//Ÿ���� ũ�ν����
	void DisplayTargetingCrossHair();

	void RemoveTargetingCrossHair();

	void ToggleTargetingCrossHair();

	bool bTargetingCrossHair;

	FVector EnemyLocation;
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
