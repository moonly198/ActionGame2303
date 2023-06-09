// Fill out your copyright notice in the Description page of Project Settings.


#include "Mutant.h"
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

// Sets default values
AMutant::AMutant()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	LeftCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCombatCollision"));
	LeftCombatCollision->SetupAttachment(GetMesh(), FName("LeftEnemySocket"));

	RightCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCombatCollision"));
	RightCombatCollision->SetupAttachment(GetMesh(), FName("RightEnemySocket"));

	FootCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCombatCollision"));
	FootCombatCollision->SetupAttachment(GetMesh(), FName("FootEnemySocket"));


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

	MutantMovementStatus = EMutantMovementStatus::MMS_Idle;

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
void AMutant::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Main)
	{
		//시작시 메인캐릭터의 타케팅 박스콜리전만 무시 
		Main->TargetingBoxCollision->IgnoreActorWhenMoving(Main, true);
		//Main->TargetingCameraBoxCollision->IgnoreActorWhenMoving(Main, true);
	}



	/*
	// AgroSphere 컴포넌트의 Collision Preset을 OverlapAllDynamic으로 변경합니다.
	AgroSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	// 이미 오버랩된 액터들의 배열을 가져옵니다.
	TArray<AActor*> OverlappingActors;
	AgroSphere->GetOverlappingActors(OverlappingActors);

	// 오버랩된 액터들의 처리를 수행합니다.
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor->IsA<AMain>())
		{
			AMain* Main = Cast<AMain>(Actor);
			if (Main)
			{
				// Initialize CombatTarget, bHasValidTarget, MainPlayerController, and move to target
				// 적 체력바
				CombatTarget = Main;
				bHasValidTarget = true;
				Main->SetHasCombatTarget(true);
				Main->SetCombatTarget(this);

				if (Main->MainPlayerController)
				{
					Main->MainPlayerController->DisplayEnemyHealthBar();
				}

				MoveToTarget(Main);
			}
		}
	}
	*/

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AMutant::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AMutant::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AMutant::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AMutant::CombatSphereOnOverlapEnd);

	LeftCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMutant::CombatOnOverlapBegin);
	LeftCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMutant::CombatOnOverlapEnd);

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMutant::CombatOnOverlapBegin);
	RightCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMutant::CombatOnOverlapEnd);

	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	FootCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMutant::CombatOnOverlapBegin);
	FootCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMutant::CombatOnOverlapEnd);

	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	FootCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FootCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
}

// Called every frame
void AMutant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MutantMovementStatus == EMutantMovementStatus::MMS_Dead) return;

	if (Health <= 0)
		Die();
	
	//스턴이 한번 된 후 스턴이 아니게되면 
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


FRotator AMutant::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMutant::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

// Called to bind functionality to input
void AMutant::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMutant::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		
		if (Main)
		{
			bAgroShpehreOverlap = true;
			
			bHasValidTarget = true;
			
			// 적 체력바
			if (Main->MainPlayerController)
			{
				Main->MainPlayerController->DisplayEnemyHealthBar();
			}

			MoveToTarget(Main);
			
		}
	}
}

void AMutant::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		
		{
			if (Main)
			{
				bAgroShpehreOverlap = false;
				bHasValidTarget = false;
				

				// 적 체력바
				//if (Main->CombatTarget == this && !Main->bTargetingBoxOverlap)
				{
				//	UE_LOG(LogTemp, Warning, TEXT("combattarget xxxxxxx!!!!!!!!!!!"));
				//	Main->SetCombatTarget(nullptr);
				//	Main->SetHasCombatTarget(false);
				}

				if (Main->MainPlayerController)
				{
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}

				SetMutantMovementStatus(EMutantMovementStatus::MMS_Idle);
				if (AIController)
				{
					AIController->StopMovement();
				}
			}
		}
	}
}

void AMutant::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = true;
				CombatTarget = Main;

				FVector MainLocation = Main->GetActorLocation();
				FVector MutantLocation = this->GetActorLocation();
				float DistanceToMain = FVector::Distance(MainLocation, MutantLocation);

				Attack();	
			}
		}
	}
}

void AMutant::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				bOverlappingCombatSphere = false;
				if (!bStunned && !bCriticalStunned &&!bAttacking)
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

void AMutant::MoveToTarget(AMain* Target)
{
	
	UE_LOG(LogTemp, Warning, TEXT("MoveToTarget"));

	if (AIController)
	{

		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(10.0f); // 두 콜리전 사이 거리

		FNavPathSharedPtr NavPath;
		
		SetMutantMovementStatus(EMutantMovementStatus::MMS_MoveToTarget);
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

void AMutant::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		
		if (Main)
		{
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				const USkeletalMeshSocket* PunchSocket = GetMesh()->GetSocketByName("PunchSocket");
				const USkeletalMeshSocket* FootSocket = GetMesh()->GetSocketByName("FootEnemySocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
				else if (PunchSocket)
				{
					FVector SocketLocation = PunchSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
				else if (FootSocket)
				{
					FVector SocketLocation = FootSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
					
				}
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

void AMutant::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMutant::ActivateCollision()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("MutantAttack_Swiping"))
		{
			LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			
		}
		else if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("MutantAttack_Punch"))
		{
			RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			
		}
		else if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("MutantAttack_Jumping") || 
				AnimInstance->Montage_GetCurrentSection(CriticalCombatMontage) == (FName("MutantAttack_JumpAttack")))
		{
			FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		}
	}
	
	
	/*
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
	*/
	//각 공격시 소리
	if (LeftCombatCollision->IsQueryCollisionEnabled())
	{
		UGameplayStatics::PlaySound2D(this, SwipingAttackSound);
	}
	else if (RightCombatCollision->IsQueryCollisionEnabled())
	{
		UGameplayStatics::PlaySound2D(this, PunchAttackSound);
	}
	else if (FootCombatCollision->IsQueryCollisionEnabled())
	{
		UGameplayStatics::PlaySound2D(this, SmallJumpAttackSound);
	}
}

void AMutant::DeactivateCollision()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutant::Attack()
{
	if (Alive() && bHasValidTarget && !bStunned && !bCriticalStunned)
	{
		
		if (!bAttacking)
		{
			if (AIController)
			{
				AIController->StopMovement();
				SetMutantMovementStatus(EMutantMovementStatus::MMS_Attacking);
			}
			UE_LOG(LogTemp, Warning, TEXT("enemyAttack!"));
			bAttacking = true;
			SetInterpToEnemy(true);

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

			FVector MainLocation = Main->GetActorLocation();
			FVector MutantLocation = this->GetActorLocation();
			float DistanceToMain = FVector::Distance(MainLocation, MutantLocation);

			if (AnimInstance)
			{
				/*
				if (DistanceToMain >= 500)
				{
					UE_LOG(LogTemp, Warning, TEXT("Attack3"));
					AnimInstance->Montage_Play(CombatMontage, BigJumpSpeed);
					AnimInstance->Montage_JumpToSection(FName("MutantAttack_JumpAttack"), CombatMontage);
					return;
				}
				*/
				int32 Section = FMath::RandRange(0, 3);
				switch (Section)
				{
					case 0:
						UE_LOG(LogTemp, Warning, TEXT("Attack0"));
						//if (DistanceToMain <= 100)
						{
							SetInterpToEnemy(false);
							AnimInstance->Montage_Play(CombatMontage, PunchSpeed);
							AnimInstance->Montage_JumpToSection(FName("MutantAttack_Punch"), CombatMontage);
						}
						

						break;

					case 1:
						UE_LOG(LogTemp, Warning, TEXT("Attack1"));
						//if (DistanceToMain <= 100)
						{
							SetInterpToEnemy(false);
							AnimInstance->Montage_Play(CombatMontage, SwipingSpeed);
							AnimInstance->Montage_JumpToSection(FName("MutantAttack_Swiping"), CombatMontage);
						}
						
						
						
						break;
					case 2:
						UE_LOG(LogTemp, Warning, TEXT("Attack2"));
						//if (DistanceToMain <= 100)
						{
							SetInterpToEnemy(false);
							AnimInstance->Montage_Play(CombatMontage, SmallJumpSpeed);
							AnimInstance->Montage_JumpToSection(FName("MutantAttack_Jumping"), CombatMontage);
						}
						
						break;
						
					case 3:
						UE_LOG(LogTemp, Warning, TEXT("Attack3"));
						//if (DistanceToMain >= 500) 
						{
							bCriticalAttack = true;
							AnimInstance->Montage_Play(CriticalCombatMontage, BigJumpSpeed);
							AnimInstance->Montage_JumpToSection(FName("MutantAttack_JumpAttack"), CriticalCombatMontage);
						}
						
						break;
						
					default:
						;
				}

				

				
			}
		}
	}
}

void AMutant::AttackEnd()
{
	bAttacking = false;
	bCriticalAttack = false;

	UE_LOG(LogTemp, Warning, TEXT("Mutant ATtack ENd!"));
	
	//CombatSphereOverlapEnd 가 실행이 됬는데 EMS_Attacking 일 경우에는 AttackEnd()에서 한번더 검사
	if (!bOverlappingCombatSphere && MutantMovementStatus == EMutantMovementStatus::MMS_Attacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("nono!"));
		AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
		MoveToTarget(Main);
	}
	
	if(bOverlappingCombatSphere)
	{
		//AttackTime은 AttackMinTime과 AttackMaxTime 사이의 랜덤 숫자
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		//AttackTime 후 에 Attack()함수 호출
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AMutant::Attack, AttackTime);
	}
}

float AMutant::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
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
		if(!bStunned && !bCriticalStunned)
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
		if(!bCriticalStunned)
			CriticalStunned();
		
	}
	else
	{	
		//위젯에서 줄어드는 것처럼 보이기 위해 프로그래스 바를 오른쪽에서 왼쪽으로 차게한다음 
		//CurrentMaxStamina에 Main->StaminaDamage를 더해주어 줄어드는것 처럼 보이게 함. 
		CurrentMaxStamina += DamageAmount;
	}
	
	
	if (bStunned)
	{
		Rigid();
	}


	//0.5초 뒤에 bTakeDamage = false;를 해줌 왜?
	FTimerHandle aa;
	GetWorld()->GetTimerManager().SetTimer(aa, FTimerDelegate::CreateLambda([&]()
		{
			bTakeDamage = false;
		}), 0.5f, false);

	return DamageAmount;
}

void AMutant::DamagedStamina()
{
	
}

void AMutant::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("MutantDie"), CombatMontage);

	}
	SetMutantMovementStatus(EMutantMovementStatus::MMS_Dead);

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutant::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	//DeathDelay 시간 후 Disappear함수 호출
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AMutant::Disappear, DeathDelay);
}

bool AMutant::Alive()
{
	return GetMutantMovementStatus() != EMutantMovementStatus::MMS_Dead;
}

void AMutant::Disappear()
{
	Destroy();
}


void AMutant::Stunned()
{
	UE_LOG(LogTemp, Warning, TEXT("MutantStun!"));
	MutantMovementStatus = EMutantMovementStatus::MMS_Stun;
	AttackEnd();
	//bAttacking = false;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	class UEnemyAnimInstance* EnemyAnimInstance;
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	if (EnemyAnimInstance != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, StunSound);
		bStunned = true;

		if (AnimInstance && StunMontage)
		{
			AnimInstance->Montage_Play(StunMontage, 1);
			AnimInstance->Montage_JumpToSection(FName("Mutant_Stunned_Idle"), StunMontage);

			GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
			{
				bStunned = false;
				//Main 캐스팅을 람다함수 전에 만들면 오류가 남
				//AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
				//MoveToTarget(Main);
			}), StunTime, false);
		}
	}
	bWasStunned = true;
}

void AMutant::CriticalStunned()
{
	if (bStunned)
	{
		bStunned = false;
		bWasStunned = false;

	}
	//bAttacking = false;
	UE_LOG(LogTemp, Warning, TEXT("MutantCriticalStun!"));
	//CombatTarget = nullptr;
	AttackEnd();
	MutantMovementStatus = EMutantMovementStatus::MMS_CriticalStun;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	class UEnemyAnimInstance* EnemyAnimInstance;
	EnemyAnimInstance = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	if (EnemyAnimInstance != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, CriticalStunSound);
		bCriticalStunned = true;
		
		if (AnimInstance && StunMontage)
		{
			if (!bCriticalStunnedAnimPlay)
			{
				AnimInstance->Montage_Play(StunMontage, 1);
				AnimInstance->Montage_JumpToSection(FName("Mutant_Critical_Stunned"), StunMontage);
				bCriticalStunnedAnimPlay = true;
			}
			
			
		}
			GetWorld()->GetTimerManager().SetTimer(StunTimer, FTimerDelegate::CreateLambda([&]()
				{
					//bCriticalStunned = false; // anim_bp 에서 해줌
					bCriticalStunnedAnimPlay = false;
					CurrentMaxStamina = 0.f;
					Stamina = MaxStamina;
					/*
					if (bOverlappingCombatSphere)
					{
						Attack();
					}
					else
					{
						AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
						MoveToTarget(Main);
					}
					*/
				}), CriticalStunTime, false);
	}
}

//스턴때 맞으면 경직
void AMutant::Rigid()
{
	//EnemyMovementStatus = EEnemyMovementStatus::EMS_Beaten;
	//bBeaten = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		//스턴상태일때만 경직 먹기
		if (MutantMovementStatus == EMutantMovementStatus::MMS_Stun)
		{
			AnimInstance->Montage_Play(StunMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("Mutant_HitReact"), CombatMontage);

			AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			const FVector MainForwordDir = Main->GetActorRotation().Vector(); // 메인캐릭터가 가고있는 앞쪽방향
			LaunchCharacter((MainForwordDir * AttackedDistance) / 2, true, false);
		}

	}
	
	
}