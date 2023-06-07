// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Mutant_test.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME2303_API AMutant_test : public AEnemy
{
	GENERATED_BODY()


public:

	AMutant_test();


	//각각 공격 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		class UBoxComponent* LeftCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		UBoxComponent* RightCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		UBoxComponent* FootCombatCollision;


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

	
	void ActivateCollision()override;
	
	void DeactivateCollision()override;


	void Die()override;

};
