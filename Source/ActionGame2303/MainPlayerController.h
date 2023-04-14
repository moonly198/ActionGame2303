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

	//에디터에서 UMG참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "widgets")
		TSubclassOf <class UUserWidget> HUDOverlayAsset;

	//위젯을 생선한 후 위젯을 담을 변수
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
	
protected:
	virtual void BeginPlay() override;

};
