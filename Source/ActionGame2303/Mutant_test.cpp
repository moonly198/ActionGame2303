// Fill out your copyright notice in the Description page of Project Settings.


#include "Mutant_test.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Main.h"
#include "MainPlayerController.h"

AMutant_test::AMutant_test()
{
	LeftCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCombatCollision"));
	LeftCombatCollision->SetupAttachment(GetMesh(), FName("LeftEnemySocket"));

	RightCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCombatCollision"));
	RightCombatCollision->SetupAttachment(GetMesh(), FName("RightEnemySocket"));

	FootCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FootCombatCollision"));
	FootCombatCollision->SetupAttachment(GetMesh(), FName("FootEnemySocket"));

}

void AMutant_test::BeginPlay()
{
	Super::BeginPlay();
	/*
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	AMainPlayerController* MPC = Cast<AMainPlayerController>(GetWorld()->GetFirstPlayerController());
	if (MPC)
	{
		UE_LOG(LogTemp, Warning, TEXT(" DisplayEnemyHealthBar"));
		MPC->DisplayEnemyHealthBar();
	}
	else if (MPC == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("MPC Null"));
	}
	*/

	LeftCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	LeftCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	RightCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	FootCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	FootCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	FootCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FootCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void AMutant_test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AMutant_test::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy::CombatSphereOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)
			{
				Main->MainPlayerController->DisplayEnemyHealthBar();
				Attack();
			}
		}
	}
}

void AMutant_test::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy::CombatSphereOnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}


void AMutant_test::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy::CombatOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMutant_test::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMutant_test::Attack()
{
	AEnemy::Attack();
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FVector MainLocation = Main->GetActorLocation();
	FVector MutantLocation = this->GetActorLocation();
	float DistanceToMain = FVector::Distance(MainLocation, MutantLocation);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance)
	{

		int32 Section = FMath::RandRange(0, 3);
		switch (Section)
		{
		case 0:
			UE_LOG(LogTemp, Warning, TEXT("Attack0"));
			//if (DistanceToMain <= 100)
			{
				SetInterpToEnemy(false);
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("MutantAttack_Punch"), CombatMontage);
			}


			break;

		case 1:
			UE_LOG(LogTemp, Warning, TEXT("Attack1"));
			//if (DistanceToMain <= 100)
			{
				SetInterpToEnemy(false);
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("MutantAttack_Swiping"), CombatMontage);
			}



			break;
		case 2:
			UE_LOG(LogTemp, Warning, TEXT("Attack2"));
			//if (DistanceToMain <= 100)
			{
				SetInterpToEnemy(false);
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("MutantAttack_Jumping"), CombatMontage);
			}

			break;

		case 3:
			UE_LOG(LogTemp, Warning, TEXT("Attack3"));
			if (DistanceToMain >= 500) 
			{
				bCriticalAttack = true;
				AnimInstance->Montage_Play(CriticalCombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("MutantAttack_JumpAttack"), CriticalCombatMontage);
			}
			AttackEnd(); //임시
			break;

		default:
			;
		}
	}
}

void AMutant_test::Stunned()
{
	AEnemy::Stunned();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(StunMontage, 1);
		AnimInstance->Montage_JumpToSection(FName("Mutant_Stunned_Idle"), StunMontage);
	}
	
}

void AMutant_test::CriticalStunned()
{
	AEnemy::CriticalStunned();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		if (!bCriticalStunnedAnimPlay)
		{
			AnimInstance->Montage_Play(StunMontage, 1);
			AnimInstance->Montage_JumpToSection(FName("Mutant_Critical_Stunned"), StunMontage);
			bCriticalStunnedAnimPlay = true;
		}


	}
}


void AMutant_test::ActivateCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("ActivateCollision"));
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
	/*
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
	*/
}

void AMutant_test::DeactivateCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("DeactivateCollision"));
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMutant_test::Die()
{
	AEnemy::Die();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("MutantDie"), CombatMontage);

	}

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FootCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}
