// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Mutant.h"
#include "Enemy.h"
#include "Mutant_test.h"
#include "Enemy_Sevarog.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner(); // 현재클래스의 pawn을 얻기
		if (Pawn)
		{
			
			Enemy = Cast<AEnemy>(Pawn);
			Mutant = Cast<AMutant>(Pawn);
			Mutant_test = Cast<AMutant_test>(Pawn);
			Enemy_Sevarog = Cast<AEnemy_Sevarog>(Pawn);

		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Enemy = Cast<AEnemy>(Pawn);
			Mutant = Cast<AMutant>(Pawn);
			Mutant_test = Cast<AMutant_test>(Pawn);
			Enemy_Sevarog = Cast<AEnemy_Sevarog>(Pawn);
		}
	}
	if (Pawn)
	{

		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

	}
}


