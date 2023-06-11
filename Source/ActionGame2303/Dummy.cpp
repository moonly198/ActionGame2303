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
		//시작시 메인캐릭터의 타케팅 박스콜리전만 무시 
		Main->TargetingBoxCollision->IgnoreActorWhenMoving(Main, true);
	}

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &ADummy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &ADummy::AgroSphereOnOverlapEnd);
	
}

// Called every frame
void ADummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//스턴이 한번 된 후 스턴이 아니게되면 
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



		bWasStunned = false; // bStunned = false임 bWasStunne은 스턴 함수에서 트루로 변경해줌
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
		//Stamina가 0일 시에 Enemy는 잠시 Stun 상태가 됨.
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
		//CurrentMaxStamina가 MaxStamina까지 다 차게 된다면 Enemy는 강공격이 들어가는 상태가 됨
		CurrentMaxStamina = MaxStamina;
		if (!bCriticalStunned)
			CriticalStunned();

	}
	else
	{
		//위젯에서 줄어드는 것처럼 보이기 위해 프로그래스 바를 오른쪽에서 왼쪽으로 차게한다음 
		//CurrentMaxStamina에 Main->StaminaDamage를 더해주어 줄어드는것 처럼 보이게 함. 
		CurrentMaxStamina += DamageAmount + 50;
	}

	//0.5초 뒤에 bTakeDamage = false;를 해줌 왜?
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

			// 적 체력바
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

				// 적 체력바
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
