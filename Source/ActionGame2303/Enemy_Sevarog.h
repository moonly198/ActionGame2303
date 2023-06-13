// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Enemy_Sevarog.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME2303_API AEnemy_Sevarog : public AEnemy
{
	GENERATED_BODY()


public:

	AEnemy_Sevarog();


	//각각 공격 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		class UBoxComponent* HandCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		UBoxComponent* HammerCollision;

	//각 공격 소리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		class USoundCue* Attack1Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* Attack2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* Attack3Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* Attack4Sound;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;


	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;


	void Attack()override;

	void Stunned()override;

	void CriticalStunned()override;

	UFUNCTION(BlueprintCallable)
		void ActivateCollision();

	UFUNCTION(BlueprintCallable)
		void DeactivateCollision();


	void Die()override;

	void Rigid()override;

	void PlaySound();
	
};
