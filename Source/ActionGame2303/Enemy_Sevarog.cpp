// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy_Sevarog.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Main.h"
#include "MainPlayerController.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"


AEnemy_Sevarog::AEnemy_Sevarog()
{
	HandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HandCollision"));
	HandCollision->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	HammerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HammerCollision"));
	HammerCollision->SetupAttachment(GetMesh(), FName("HammerCenter"));

}


void AEnemy_Sevarog::BeginPlay()
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

	HandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	HandCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	HandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HammerCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	HammerCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	HammerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HammerCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	HammerCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HammerCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called every frame
void AEnemy_Sevarog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemy_Sevarog::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AEnemy_Sevarog::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEnemy::CombatSphereOnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}


void AEnemy_Sevarog::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy::CombatOnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	AMain* Main = Cast<AMain>(OtherActor);

	if (Main)
	{

		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName("WeaponHandSocket");
		const USkeletalMeshSocket* HammerSocket = GetMesh()->GetSocketByName("HammerCenter");
		if (HandSocket)
		{
			FVector SocketLocation = HandSocket->GetSocketLocation(GetMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
		}
		else if (HammerSocket)
		{
			FVector SocketLocation = HammerSocket->GetSocketLocation(GetMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
		}
	}
}

void AEnemy_Sevarog::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy_Sevarog::Attack()
{
	AEnemy::Attack();
	AMain* Main = Cast<AMain>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	FVector MainLocation = Main->GetActorLocation();
	FVector MutantLocation = this->GetActorLocation();
	float DistanceToMain = FVector::Distance(MainLocation, MutantLocation);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (Alive() && bHasValidTarget && !bStunned && !bCriticalStunned)
	{
		if (AnimInstance)
		{

			int32 Section = FMath::RandRange(0, 4);
			switch (Section)
			{
			case 0:
				{
					SetInterpToEnemy(false);
					AnimInstance->Montage_Play(CombatMontage, 1.f);
					AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
				}


				break;
			case 1:
				{
					SetInterpToEnemy(false);
					AnimInstance->Montage_Play(CombatMontage, 1.f);
					AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
				}
				break;
			case 2:
				{
					SetInterpToEnemy(false);
					AnimInstance->Montage_Play(CombatMontage, 1.f);
					AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
				}

				break;
			case 3:
				{
				SetInterpToEnemy(false);
					AnimInstance->Montage_Play(CombatMontage, 1.f);
					AnimInstance->Montage_JumpToSection(FName("Attack4"), CombatMontage);
				}
				break;
			case 4:
			{
				SetInterpToEnemy(false);
				bCriticalAttack = true;
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("CriticalAttack"), CombatMontage);
			}
			break;

			default:
				;
			}

		}
	}
}

void AEnemy_Sevarog::Stunned()
{
	AEnemy::Stunned();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(StunMontage, 1);
		AnimInstance->Montage_JumpToSection(FName("Stun_Start"), StunMontage);
	}

}

void AEnemy_Sevarog::CriticalStunned()
{
	AEnemy::CriticalStunned();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		if (!bCriticalStunnedAnimPlay)
		{
			AnimInstance->Montage_Play(StunMontage, 1);
			AnimInstance->Montage_JumpToSection(FName("Stun_Start"), StunMontage);
			bCriticalStunnedAnimPlay = true;
		}


	}
}


void AEnemy_Sevarog::ActivateCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("ActivateCollision"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("Attack4"))
		{
			HandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		}
		else
		{
			HammerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		}
	}


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

void AEnemy_Sevarog::DeactivateCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("DeactivateCollision"));
	HandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HammerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy_Sevarog::Die()
{
	AEnemy::Die();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);

	}

	HandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HammerCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AEnemy_Sevarog::Rigid()
{
	AEnemy::Rigid();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && StunMontage)
	{
		//스턴상태일때만 경직 먹기
		if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Stun)
		{
			AnimInstance->Montage_Play(StunMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("HitReact"), CombatMontage);
		}
	}
}

void AEnemy_Sevarog::PlaySound()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("Attack1"))
		{
			if (Attack1Sound)
			{
				UGameplayStatics::PlaySound2D(this, Attack1Sound);
			}
		}
		else if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("Attack2"))
		{
			if (Attack2Sound)
			{
				UGameplayStatics::PlaySound2D(this, Attack2Sound);
			}
		}
		else if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("Attack3"))
		{
			if (Attack3Sound)
			{
				UGameplayStatics::PlaySound2D(this, Attack3Sound);
			}
		}
		else if (AnimInstance->Montage_GetCurrentSection(CombatMontage) == FName("Attack4"))
		{
			if (Attack4Sound)
			{
				UGameplayStatics::PlaySound2D(this, Attack4Sound);
			}
		}
	}
}
