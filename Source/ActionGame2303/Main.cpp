
// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Mutant.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Creat Camera Boom(�浹�� �ִ� ��� �÷��̾ ���� ���)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f; //ī�޶� �� �Ÿ��� ������ä ����
	CameraBoom->bUsePawnControlRotation = true; //��Ʈ�ѷ� ������� ���� ȸ����

	//�ݸ��� ĸ�� ������ ���ϱ�
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	//Creat Follow Camera 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//Attach the camera to the end of the boom and let the boom adjust to match
	//the controller orientation (ī�޶� �� ���� �����ϰ� ���� ��Ʈ�ѷ� ���⿡ �°� �����ǵ��� �Ѵ�)
	FollowCamera->bUsePawnControlRotation = false;
	//ȸ���� ����
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;


	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("Sword_Strike"));

	//Don't rotate when the controller rotates.
	//Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); //...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 100.f; //���¹̳� �����ϴ� ����
	MinSprintStamina = 300.f; //���¹̳� ��� �ߴ� ��

	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	WalkingSpeed = 300.f;


	bShiftKeyDown = false;
	bCtrlKeyDown = false;

	bDashing = false;
	
	bLMBDown = false;
	bRMBDown = false;
	bESCDown = false;

	CurrentComboCount = 0;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;

	bMovingForward = false;
	bMovingRight = false;

	
}


// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMain::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMain::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DeltaStamina = StaminaDrainRate * DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("%f"),Stamina);

	switch (StaminaStatus)
	{
		case EStaminaStatus::ESS_Normal:
			if ((bShiftKeyDown || bDashing )&& !bCtrlKeyDown) // �޸��ų� �뽬�Ҷ�, ���������� BelowMinimum���� ���°� ����
			{
				if (Stamina - DeltaStamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else if(!bShiftKeyDown || !bDashing || bCtrlKeyDown) //shift Key up Ȥ�� �뽬 ���Ҷ�
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				if(!bCtrlKeyDown)
					SetMovementStatus(EMovementStatus::EMS_Normal);
				else
					SetMovementStatus(EMovementStatus::EMS_Walking);
			}
			break;

		case EStaminaStatus::ESS_BelowMinimum:
			if ((bShiftKeyDown || bDashing) && !bCtrlKeyDown)
			{
				if (Stamina - DeltaStamina <= 0.f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				//SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else if (!bShiftKeyDown || !bDashing || bCtrlKeyDown) // shift Key up
			{
				if (Stamina + DeltaStamina >= MinSprintStamina)
				{
					
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
			}

			break;

		case EStaminaStatus::ESS_Exhausted:
			if ((bShiftKeyDown || bDashing) && !bCtrlKeyDown)
			{
				Stamina = 0.f;
			}
			else if (!bShiftKeyDown || !bDashing || bCtrlKeyDown)// shift Key up
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}

			if(!bCtrlKeyDown)
				SetMovementStatus(EMovementStatus::EMS_Normal);
			else
				SetMovementStatus(EMovementStatus::EMS_Walking);
			break;

		case EStaminaStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;

			}
			else
			{
				Stamina += DeltaStamina;
			}

			if (!bCtrlKeyDown)
				SetMovementStatus(EMovementStatus::EMS_Normal);
			else
				SetMovementStatus(EMovementStatus::EMS_Walking);

			break;

		default:
			;

	}


	//�뽬��Ÿ�ְ� �������� bDashing�� true�϶��� �� �����Ӹ��� �Ǵ� 
	//�뽬 ��Ÿ�ְ� ���������� �޸��� ����
	
	if (bDashing)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DashMontage)
		{
			if (!AnimInstance->Montage_IsPlaying(DashMontage))
			{
				bDashing = false;
			}
		}
	}

	if (bAttacking)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		//MontageEnd = AnimInstance->Montage_GetPosition(CombatMontage);
		ShiftKeyUp();
		/*
		if (!AnimInstance->Montage_IsPlaying(CombatMontage))
			bMontageEnd = true;
		else
			bMontageEnd = false;
		*/
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}
	

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);

	PlayerInputComponent->BindAction("Targeting",IE_Pressed, this, &AMain::TargetingMode);
	PlayerInputComponent->BindAction("Targeting",IE_Released, this, &AMain::TravelMode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMain::Dashing);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
	
	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMain::CtrlKeyDown);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMain::CtrlKeyUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMain::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMain::RMBUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if (MovementStatus == EMovementStatus::EMS_Normal)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

	}
	else if (MovementStatus == EMovementStatus::EMS_Walking)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::CtrlKeyDown()
{
	bCtrlKeyDown = true;
	SetMovementStatus(EMovementStatus::EMS_Walking);
}

void AMain::CtrlKeyUp()
{
	bCtrlKeyDown = false;
	SetMovementStatus(EMovementStatus::EMS_Normal);
}

void AMain::Dashing()
{
	UE_LOG(LogTemp, Warning, TEXT("Dash!"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	class UMainAnimInstance* MainAnimInstance;
	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());
	if (MainAnimInstance != nullptr)
	{
		if ((!bDashing) && !(MainAnimInstance->bIsInAir) && (Stamina >= DashStamina) && (!bAttacking)) // UMainAnimInstanceŬ������ bIsInAir�� �� ���߿� �������� �뽬�� ���ϰ� ��
		{
			const FVector ForwardDir = this->GetActorRotation().Vector(); // �뽬 ����
			LaunchCharacter(ForwardDir * DashDistance, true, false);  //�뽬
			Stamina -= DashStamina;
			bDashing = true;

			if (AnimInstance && DashMontage)
			{
				AnimInstance->Montage_Play(DashMontage, 1);

			}

		}
	}
	
}


void AMain::DecrementHealth(float Damage)
{
	if (Health - Damage <= 0.f)
	{
		Health -= Damage;
		Die();
	}
	else
	{
		Health -= Damage;
	}
}

void AMain::TargetingMode()
{

}

void AMain::TravelMode()
{

}

void AMain::LMBDown()
{
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;

	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (bFirstClick)
	{
		Attack();
		bFirstClick = false;
		LastLeftClickTime = CurrentTime;
		
	}
	else if ((!bLMBDown)&&(CurrentTime - LastLeftClickTime < ClickInterval) && !LastAttack)// && !bMontageEnd)
	{
		// Double click
		
		NextAttackAnim();
		LastLeftClickTime = CurrentTime;
	}
	else
	{
		// Single click
		Attack();
	}
	
	
	
	bLMBDown = true;
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
	if (!bAttacking)
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentComnoCount : %d"), CurrentComboCount);
		bAttacking = true;
		
		
		//�ִϸ��̼� ��Ÿ�� ����ϴ¹�
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if (CurrentComboCount == 0)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
				
				
			}
			else if (CurrentComboCount == 1)
			{
				
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				
			}
			else if (CurrentComboCount == 2)
			{
				
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				
				LastAttack = true;
			}

			
				
		}
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	bFirstClick = true;
	LastAttack = false;
	CurrentComboCount = 0;
	if (bLMBDown)
	{
		Attack();
	}
	
}


void AMain::NextAttackAnim()
{
	
	bAttacking = false;
	CurrentComboCount++;
	Attack();
}

void AMain::PlaySwingSound()
{
	
		UGameplayStatics::PlaySound2D(this,SwordSwingSound);
		UGameplayStatics::PlaySound2D(this,CharSwingSound);
	
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}


void AMain::Die()
{

}

void AMain::RMBDown()
{

}

void AMain::RMBUp()
{

}


void AMain::ESCDown()
{
	bESCDown = true;
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}




void AMain::Jump()
{
	
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;
	
	if (bDashing || bAttacking)
		return;

	Super::Jump();
	
}



bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) &&
			 (!bDashing) &&
			(!bAttacking) &&
			!MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		//������ ���� ���� ���� (const�� �ٲ��� ���� �Ϳ� ���)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	if (CanMove(Value))
	{
		//������ ���� ���� ���� (const�� �ٲ��� ���� �Ϳ� ���)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);


	}
}

void AMain::Turn(float Value)
{
	if (CanMove(Value))
	{
		AddControllerYawInput(Value);
	}
}

void AMain::LookUp(float Value)
{
	if (CanMove(Value))
	{
		AddControllerPitchInput(Value);
	}
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMutant* Enemy = Cast<AMutant>(OtherActor);
		if (Enemy)
		{
			if (Enemy->HitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName("Sword_Blood");
				if (WeaponSocket)
				{
					//actor(MyProject��Weapon.cpp)�� Skeletalmesh�� �ް� char�� GetMesh()
					FVector SocketLocation = WeaponSocket->GetSocketLocation(GetMesh()); 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}

		}
	}
}

void AMain::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMain::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AMain::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
