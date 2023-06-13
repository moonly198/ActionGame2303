// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Dummy.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME2303_API ADummy : public AEnemy
{
	GENERATED_BODY()
public:
	ADummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;



};
