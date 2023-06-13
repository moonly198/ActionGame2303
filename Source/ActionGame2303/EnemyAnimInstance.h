// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME2303_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:


	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)//청사진에서 호출 가능하게 함
		void UpdateAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class AMutant* Mutant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class AMutant_test* Mutant_test;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class AEnemy* Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		class AEnemy_Sevarog* Enemy_Sevarog;
};
