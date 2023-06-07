// Fill out your copyright notice in the Description page of Project Settings.


#include "Mutant_test.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Main.h"

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

}

void AMutant_test::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


void AMutant_test::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AMutant_test::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMutant_test::Attack()
{

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
			//if (DistanceToMain >= 500) 
			{
				bCriticalAttack = true;
				AnimInstance->Montage_Play(CriticalCombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("MutantAttack_JumpAttack"), CriticalCombatMontage);
			}

			break;

		default:
			;
		}
	}
}

void AMutant_test::Stunned()
{

}

void AMutant_test::CriticalStunned()
{

}


void AMutant_test::ActivateCollision()
{

}

void AMutant_test::DeactivateCollision()
{

}

void AMutant_test::Die()
{
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
