
// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

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

	bShiftKeyDown = false;

	bDashing = false;
	
}


// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
	
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
			if (bShiftKeyDown)
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
			else //shift Key up
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;

		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown)
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
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else // shift Key up
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
			if (bShiftKeyDown)
			{
				Stamina = 0.f;
			}
			else // shift Key up
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
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
			SetMovementStatus(EMovementStatus::EMS_Normal);

			break;

		default:
			;

	}

	if (bDashing)
	{
		USkeletalMeshComponent* MainSkeletalMeshComponent = GetMesh();
		UAnimInstance* AnimInstance = MainSkeletalMeshComponent->GetAnimInstance(); // �ִ� �ν��Ͻ� ����

		if (AnimInstance && DashMontage)
		{
			if (!AnimInstance->Montage_IsPlaying(DashMontage))// ��Ÿ�� �÷��̰� �ƴҶ���
			{
				AnimInstance->Montage_Play(DashMontage);
			}


			UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage(); //���� �ִϸ��̼� ��Ÿ�� ������������
			FString Name = CurrentMontage->GetName();
			UE_LOG(LogTemp, Warning, TEXT("%s"), &Name);
			float MontageLength = CurrentMontage->GetPlayLength(); // ��Ÿ�� �ִ� ��� �ð�Ȯ��
			float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentMontage); // ��Ÿ�� ���� ����ð� Ȯ�� �ѹ��� �Ǵ� �� ����
			UE_LOG(LogTemp, Warning, TEXT("%f"), &CurrentPosition);
			if (CurrentPosition >= MontageLength) //���� ���� ����ð��� ��Ÿ�� ���̺��� ���ų� ���
			{
				bDashing = false;
			}

		}

	}

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
	//PlayerInputComponent->BindAction("Dash", IE_Released, this, &AMain::StopDash);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

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

void AMain::TargetingMode()
{

}

void AMain::TravelMode()
{

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

void AMain::Dashing()
{
	if (!bDashing)
	{
		const FVector ForwardDir = this->GetActorRotation().Vector();
		LaunchCharacter(ForwardDir * DashDistance, true, false);  //�뽬
		bDashing = true;

		

	}
	
}

void AMain::StopDash()
{
	bDashing = false;
}


void AMain::Attack()
{

}

void AMain::AttackEnd()
{

}

void AMain::Die()
{

}


void AMain::Jump()
{
	/*
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;
	*/
	if (!bDashing)
		return;
	Super::Jump();
	
}



bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) &&
			 (!bDashing); //&&
			//(!bAttacking) &&
			//!MainPlayerController->bPauseMenuVisible;
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

