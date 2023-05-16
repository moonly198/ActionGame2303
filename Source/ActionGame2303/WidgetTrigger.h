// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetTrigger.generated.h"

UENUM(BlueprintType)
enum class EWidgetTriggerStatus :uint8
{
	WTS_Time	UMETA(DeplayName = "Time"),
	WTS_Press	UMETA(DeplayName = "Press"),

	WTS_MAX		UMETA(DeplayName = "DefaultMax")

};

UCLASS()
class ACTIONGAME2303_API AWidgetTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWidgetTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWidgetTriggerStatus WidgetTriggerStatus;

	void SetWidgetTriggerStatus(EWidgetTriggerStatus Status);

	EWidgetTriggerStatus GetWidgetTriggerStatus();

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UUserWidget> WidgetAsset;


	UPROPERTY(EditAnywhere)
		UUserWidget* Widget;

	UPROPERTY(EditAnywhere)
		int32 widgetStatus;

	UPROPERTY(EditAnywhere)
		float HiddenWaitTime = 2;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
