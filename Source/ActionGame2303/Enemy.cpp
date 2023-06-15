// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemyAnimInstance.h"
#include "Components/AudioComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	StunAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StunAudio"));
	StunAudioComponent->SetupAttachment(GetRootComponent());

	bOverlappingCombatSphere = false;

	Health = 1000.f;
	MaxHealth = 1000.f;

	Stamina = 500.f;
	CurrentMaxStamina = 0.f;
	MaxStamina = 500.f;

	StaminaDrainRate = 50.f;

	EnemyHealthDamage = 10.f;
	EnemyCriticalDamage = 100.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	StunTime = 5.f;
	CriticalStunTime = 5.f;

	DeathDelay = 1.f;

	bHasValidTarget = false;

	bBeaten = false;
	AttackedDistance = 100.f;

	bCriticalStunnedAnimPlay = false;
	//InterpSpeed = 15.f;
	//bInterpToEnemy = false;

}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Main)
	{
		Main->TargetingBoxCollision->IgnoreActorWhenMoving(Main, true);
		Main->CombatTarget = this;
		MoveToTarget(Main);
	}

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Dead) return;

	if (CombatTarget && CombatTarget->MovementStatus == EMovementStatus::EMS_Dead) return;

	if (Health <= 0)
		Die();

	//스턴이 한번 된 후 스턴이 아니게되면 (스태미나를 회복하면)
	if (bWasStunned && !bStunned)
	{
		if (bCriticalStunned)
			return;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && StunMontage)
		{
			if (AnimInstance->Montage_IsPlaying(StunMontage))
			{
				AnimInstance->Montage_Stop(0.3f, StunMontage);
			}
		}

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

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

}


FRotator AEnemy::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AEnemy::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = true;
				CombatTarget = Main;
				bHasValidTarget = true;

				FVector MainLocation = Main->GetActorLocation();
				FVector MutantLocation = this->GetActorLocation();
				float DistanceToMain = FVector::Distance(MainLocation, MutantLocation);
				UE_LOG(LogTemp, Warning, TEXT("Parent Class"));

				//Attack();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = false;
				if (!bStunned && !bCriticalStunned && !bAttacking)
				{
					CombatTarget = nullptr;
					MoveToTarget(Main);
				}
				//오버랩 끝날시 공격 타이머 리셋
				GetWorldTimerManager().ClearTimer(AttackTimer);
				//EnemyAnim_BP에서 MoveToTarget함수 호출 
				//Attack();
			}
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{

	UE_LOG(LogTemp, Warning, TEXT("MoveToTarget"));

	if (AIController)
	{

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f); // 두 콜리전 사이 거리

		FNavPathSharedPtr NavPath;

		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
		AIController->MoveTo(MoveRequest, &NavPath); //NavPath의 동일한 주소를 따라가야함

		/* navPath 경로에 구를 설치해서 목적지를 볼수 잇음
		auto PathPoints = NavPath->GetPathPoints();
		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}
		*/
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);

		if (Main)
		{
			if (Main->HitParticles)
			{
				
			}

			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}

			if (DamageTypeClass)
			{
				if (Main->MovementStatus == EMovementStatus::EMS_Stun || bCriticalAttack)
				{
					UGameplayStatics::ApplyDamage(Main, EnemyCriticalDamage, AIController, this, DamageTypeClass); //데미지 적용 Main의 TakeDamage();
				}
				else
					UGameplayStatics::ApplyDamage(Main, EnemyHealthDamage, AIController, this, DamageTypeClass); //데미지 적용 Main의 TakeDamage();
			}

		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget && !bStunned && !bCriticalStunned)
	{

		if (!bAttacking)
		{
			if (AIController)
			{
				AIController->StopMovement();
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			}
			UE_LOG(LogTemp, Warning, TEXT("Enemy Attack0000000000"));

			bAttacking = true;
			SetInterpToEnemy(true);
			if (SwingSound)
			{
				UGameplayStatics::PlaySound2D(this, SwingSound);
			}

		}
	}
}


void AEnemy::AttackEnd()
{
	bAttacking = false;
	bCriticalAttack = false;

	//CombatSphereOverlapEnd 가 실행이 됬는데 EMS_Attacking 일 경우에는 AttackEnd()에서 한번더 검사
	if (!bOverlappingCombatSphere && EnemyMovementStatus == EEnemyMovementStatus::EMS_Attacking)
	{
		AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		MoveToTarget(Main);
	}
	
	if (bOverlappingCombatSphere)
	{
		//AttackTime은 AttackMinTime과 AttackMaxTime 사이의 랜덤 숫자
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		//AttackTime 후 에 Attack()함수 호출
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{

	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	DamageAmount = Main->HealthDamage;
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
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
		Stamina -= DamageAmount;
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
		CurrentMaxStamina += DamageAmount;
	}

	//0.5초 뒤에 bTakeDamage = false;를 해줌 왜?
	FTimerHandle aa;
	GetWorld()->GetTimerManager().SetTimer(aa, FTimerDelegate::CreateLambda([&]()
		{
			bTakeDamage = false;
		}), 0.5f, false);

	return DamageAmount;
}

void AEnemy::Die()
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Main->MainPlayerController)
	{
		Main->MainPlayerController->RemoveEnemyHealthBar();
	}

	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	//DeathDelay 시간 후 Disappear함수 호출
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	Destroy();
}


void AEnemy::Stunned()
{

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Stun;
	AttackEnd();

	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//class UEnemyAnimInstance* EnemyAnimInstance;
	//EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	//if (EnemyAnimInstance != nullptr)
	//{
		if (StunSound)
		{
			if (StunAudioComponent->Sound != StunSound)
				StunAudioComponent->SetSound(StunSound);

			if (!StunAudioComponent->IsPlaying())
				StunAudioComponent->Play();

		}
		bStunned = true;

		//if (AnimInstance && StunMontage)
		//{

		GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
			{
				bStunned = false;
					
				//Main 캐스팅을 람다함수 전에 만들면 오류가 남
				//AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				//MoveToTarget(Main);	
			}), StunTime, false);
		//}
	//}
	bWasStunned = true;
}

void AEnemy::CriticalStunned()
{
	if (bStunned)
	{
		bStunned = false;
		bWasStunned = false;

	}
	
	AttackEnd();
	EnemyMovementStatus = EEnemyMovementStatus::EMS_CriticalStun;

	//UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	//class UEnemyAnimInstance* EnemyAnimInstance;
	//EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	//if (EnemyAnimInstance != nullptr)
	{
		if (StunSound)
		{
			if (StunAudioComponent->Sound != StunSound)
				StunAudioComponent->SetSound(StunSound);

			if (!StunAudioComponent->IsPlaying())
				StunAudioComponent->Play();

		}
		bCriticalStunned = true;
		GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
			{
				//bCriticalStunned = false; // anim_bp 에서 해줌
				bCriticalStunnedAnimPlay = false;
				CurrentMaxStamina = 0.f;
				Stamina = MaxStamina;
				
			}), CriticalStunTime, false);
	}
}

//스턴때 맞으면 경직
void AEnemy::Rigid()
{
	//스턴상태일때만 경직 먹기
	if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Stun)
	{
		AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		const FVector MainForwordDir = Main->GetActorRotation().Vector(); // 메인캐릭터가 가고있는 앞쪽방향
		LaunchCharacter((MainForwordDir * AttackedDistance) / 2, true, false);
	}
}

void AEnemy::PlaySound()
{
	
}