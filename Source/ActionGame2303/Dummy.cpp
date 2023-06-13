// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "AIController.h"
#include "Main.h"
#include "MainPlayerController.h"


ADummy::ADummy()
{

}
// Called when the game starts or when spawned
void ADummy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
	}

}	


void ADummy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy::CombatSphereOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->MainPlayerController->DisplayEnemyHealthBar();
				if (AIController)
				{
					AIController->StopMovement();
				}

			}
		}
	}
}

void ADummy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy::CombatSphereOnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->MainPlayerController->RemoveEnemyHealthBar();
				if (AIController)
				{
					AIController->StopMovement();
				}

			}
		}
	}
}


