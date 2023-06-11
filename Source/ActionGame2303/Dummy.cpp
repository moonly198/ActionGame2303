// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "MainPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ADummy::ADummy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);


	Health = 1000.f;
	MaxHealth = 1000.f;

	Stamina = 500.f;
	CurrentMaxStamina = 0.f;
	MaxStamina = 500.f;

	StaminaDrainRate = 50.f;

	DummyMovementStatus = EDummyMovementStatus::DMS_Idle;

}

// Called when the game starts or when spawned
void ADummy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Main)
	{
		//���۽� ����ĳ������ Ÿ���� �ڽ��ݸ����� ���� 
		Main->TargetingBoxCollision->IgnoreActorWhenMoving(Main, true);
	}

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ADummy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ADummy::AgroSphereOnOverlapEnd);
	
}

// Called every frame
void ADummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//������ �ѹ� �� �� ������ �ƴϰԵǸ� 
	if (bWasStunned && !bStunned)
	{
		if (bCriticalStunned)
			return;

		float DeltaStamina = StaminaDrainRate * DeltaTime;


		if (Stamina >= (MaxStamina - CurrentMaxStamina))
		{
			Stamina = MaxStamina - CurrentMaxStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		Stamina = (MaxStamina - CurrentMaxStamina);



		bWasStunned = false; // bStunned = false�� bWasStunne�� ���� �Լ����� Ʈ��� ��������
	}


}

// Called to bind functionality to input
void ADummy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ADummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	DamageAmount = Main->HealthDamage;
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		
	}
	else
	{

		if (bCriticalStunned && Main->bCriticalAttack)
		{
			DamageAmount = Main->CriticalHealthDamage;
			Health -= DamageAmount;
		}
		else
			Health -= DamageAmount;



	}

	DamageAmount = Main->StaminaDamage;
	//Stamina
	if (Stamina - DamageAmount <= 0.f)
	{
		//Stamina�� 0�� �ÿ� Enemy�� ��� Stun ���°� ��.
		Stamina = 0.f;
		if (!bStunned && !bCriticalStunned)
			Stunned();
	}
	else
	{
		Stamina -= DamageAmount + 100;
	}

	DamageAmount = Main->MaxStaminaDamage;

	//MaxStamina
	if (CurrentMaxStamina + DamageAmount >= MaxStamina)
	{
		//CurrentMaxStamina�� MaxStamina���� �� ���� �ȴٸ� Enemy�� �������� ���� ���°� ��
		CurrentMaxStamina = MaxStamina;
		if (!bCriticalStunned)
			CriticalStunned();

	}
	else
	{
		//�������� �پ��� ��ó�� ���̱� ���� ���α׷��� �ٸ� �����ʿ��� �������� �����Ѵ��� 
		//CurrentMaxStamina�� Main->StaminaDamage�� �����־� �پ��°� ó�� ���̰� ��. 
		CurrentMaxStamina += DamageAmount + 50;
	}

	//0.5�� �ڿ� bTakeDamage = false;�� ���� ��?
	FTimerHandle aa;
	GetWorld()->GetTimerManager().SetTimer(aa, FTimerDelegate::CreateLambda([&]()
		{
			bTakeDamage = false;
		}), 0.5f, false);

	return DamageAmount;
}

void ADummy::Stunned()
{
	
	DummyMovementStatus = EDummyMovementStatus::DMS_Stun;
	bStunned = true;
	bWasStunned = true;
	GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
		{
			bStunned = false;
			SetDummyMovementStatus(EDummyMovementStatus::DMS_Idle);

		}), StunTime, false);
}

void ADummy::CriticalStunned()
{
	if (bStunned)
	{
		bStunned = false;
		bWasStunned = false;
	}

	DummyMovementStatus = EDummyMovementStatus::DMS_CriticalStun;

	GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
		{
			CurrentMaxStamina = 0.f;
			Stamina = MaxStamina;
			SetDummyMovementStatus(EDummyMovementStatus::DMS_Idle);
			AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (Main)
			{
				Main->bCriticalAttackOnce = false;
			}
			
		}), CriticalStunTime, false);
}

void ADummy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{

			// �� ü�¹�
			if (Main->MainPlayerController)
			{
				Main->MainPlayerController->DisplayEnemyHealthBar();
			}

		}
	}
}

void ADummy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);

		{
			if (Main)
			{

				// �� ü�¹�
				//if (Main->CombatTarget == this && !Main->bTargetingBoxOverlap)
				{
				//	Main->SetCombatTarget(nullptr);
				//	Main->SetHasCombatTarget(false);
				}

				if (Main->MainPlayerController)
				{
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}

				SetDummyMovementStatus(EDummyMovementStatus::DMS_Idle);
			}
		}
	}
}
