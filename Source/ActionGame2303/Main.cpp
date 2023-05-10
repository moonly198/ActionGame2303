
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
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "HealthDamageType.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 컴포넌트를 부착할 USceneComponent 생성
	CameraSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CameraSceneComponent"));
	CameraSceneComponent->SetupAttachment(GetRootComponent());

	//Creat Camera Boom(충돌이 있는 경우 플레이어를 향해 당김)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraSceneComponent);
	CameraBoom->TargetArmLength = 400.f; //카메라가 이 거리를 유지한채 따라감
	CameraBoom->bUsePawnControlRotation = true; //컨트롤러 기반으로 팔이 회전함


	//Creat Follow Camera 
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	//TargetingCameraBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TargetingCameraBoxCollision"));
	//TargetingCameraBoxCollision->SetupAttachment(CameraBoom); // 카메라를 따라가는 콜리전 박스


	TargetingBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TargetingBoxCollision"));
	TargetingBoxCollision->SetupAttachment(CameraBoom); // 캐릭터앞에 고정되어잇는 콜리전 박스

	//Attach the camera to the end of the boom and let the boom adjust to match
	//the controller orientation (카메라를 붐 끝에 부착하고 붐이 컨트롤러 방향에 맞게 조정되도록 한다)
	FollowCamera->bUsePawnControlRotation = false;

	//회전율 설정
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;


	//콜리젼 캡슐 사이즈 정하기
	GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

	SwordCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordCombatCollision"));
	SwordCombatCollision->SetupAttachment(GetMesh(), FName("Sword_Strike"));


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

	StunAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StunAudio"));
	StunAudioComponent->SetupAttachment(GetRootComponent());
	
	MaxHealth = 1000.f;
	Health = 800.f;
	
	MaxStamina = 1000.f;
	Stamina = 800.f;

	AttackReCoverStaminaTime = 1.f;

	StaminaDrainRate = 100.f; //스태미나 감소하는 비율
	MinSprintStamina = 300.f; //스태미나 경고 뜨는 선

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

	bReadyStunned = true;

	bLockOn = false;
	targetingHeightOffset = 20.0f;
	lockedOnActor = nullptr;
	bTargetingBoxOverlap = false;
	TargetingCameraInterpSpeed = 10.f;
	TargetingCameraPitchInterpSpeed = 10.f;
	MainToLockActorDistance = 500.f;
	//bTargetingCameraBoxOverlap = false;

	bCriticalAttack = false;

	
}


// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());


	SwordCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AMain::CombatOnOverlapBegin);
	SwordCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AMain::CombatOnOverlapEnd);

	SwordCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	TargetingBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMain::TargetingBoxOnOverlapBegin);
	TargetingBoxCollision->OnComponentEndOverlap.AddDynamic(this, &AMain::TargetingBoxOnOverlapEnd);
	//TargetingBoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	//TargetingCameraBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMain::TargetingCameraBoxOnOverlapBegin);
	//TargetingCameraBoxCollision->OnComponentEndOverlap.AddDynamic(this, &AMain::TargetingCameraBoxOnOverlapEnd);

}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	//EMovementStatus::EMS_Stun 이고 스턴상태일때, bStunned으로로 안한 이유는 스턴애님이 바로 해제 됨 
	//달려서 지칠때 애님이 너무 길어서 종료하게끔 만듬, 스턴시 숨소리
	if (MovementStatus != EMovementStatus::EMS_Stun && !bReadyStunned)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && StunMontage)
		{
			if ( Stamina >= MinSprintStamina) //스태미너 빨간색 초록색으로 변했는데
			{
				if (AnimInstance->Montage_IsPlaying(StunMontage))//아직 스턴 애님이 플레이중이라면
				{
					AnimInstance->Montage_Stop(0.1f, StunMontage);
					
				}
				
			}
			/* //그냥 스태미너 고갈시 스턴애님을 길게만듬
			else//스태미너 아직 빨간색이고 스턴 애님이 끝났는데 못움직일때 
			{
				if (!AnimInstance->Montage_IsPlaying(StunMontage))
				{
					AnimInstance->Montage_Play(StunMontage, 1.f);
					
				}
			}
			*/
			bStunned = false;
			bReadyStunned = true;
			
		}
		if (StunSound)
		{
			if (StunAudioComponent->IsPlaying())
			{
				StunAudioComponent->Stop();
			}

		}
			
	}


	//스턴인데 공격을 당했을때 숨소리 멈추고, CombatMontage의 Death애님이 끝나면 일어서야함
	if (CombatStatus == ECombatStatus::ECS_StunTakeDamage)
	{

		if (StunSound)
		{
			if (StunAudioComponent->IsPlaying())
			{
				StunAudioComponent->Stop();
			}

		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if ( AnimInstance->Montage_IsPlaying(CombatMontage))
			{
				SetCombatStatus(ECombatStatus::ECS_StunTakeDamage);
			}
			else if (!AnimInstance->Montage_IsPlaying(CombatMontage))
			{
				if (Stamina > MinSprintStamina)
				{
					SetCombatStatus(ECombatStatus::ECS_Normal);
				}
			}
		}

	}

	//ECS_TakeDamage 몽타주 끝나면 노말로 넘어가야함
	if (CombatStatus == ECombatStatus::ECS_TakeDamage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
			if (!AnimInstance->Montage_IsPlaying(CombatMontage))
			{
				SetCombatStatus(ECombatStatus::ECS_Normal);
			}
		}
	}

	//대쉬몽타주가 끝났는지 bDashing이 true일때만 매 프레임마다 판단 
	//대쉬 몽타주가 끝날때까지 달리지 못함
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

	//공격시 잠시 기다렸다가 스태미나 회복 
	if (bAttacking)
		StaminaDelay(DeltaTime);
	else
		RcoveringStamina(DeltaTime);

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget-> GetMesh()-> GetSocketLocation(FName("TargetSocket"));
		//CombatTargetLocation = CombatTarget->GetActorLocation();//캐릭터 클래스 적의 로케이션 가져오기
		
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
			//UKismetSystemLibrary::DrawDebugSphere(this, MainPlayerController->EnemyLocation, 25.f, 8, FLinearColor::Red, 10.f, 1.5f);
		}

	}

	if (bCriticalAttack)
	{
		if (CombatTarget && CombatTarget->EnemyMovementStatus != EEnemyMovementStatus::EMS_CriticalStun)
		{
			bCriticalAttack = false;
		}
	}
	
	if (bInterpToEnemy && CombatTarget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CombatToTarget"));
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);
		
	}

	if (bLockOn && bTargetingBoxOverlap)
	{
		//캐릭터 회전
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(InterpRotation);

		//카메라 회전
		FVector LockActorLocation = lockedOnActor->GetActorLocation();
		FVector MainLocation = this->GetActorLocation();
		float Distance = (LockActorLocation - MainLocation).Size();

		FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), lockedOnActor->GetActorLocation());
		

		if (!bLockOn)
		{
			FRotator InterpRotation1 = FMath::RInterpTo(GetActorRotation(), lookAtRotation, DeltaTime, TargetingCameraInterpSpeed);
			GetController()->SetControlRotation(InterpRotation1); // 락온이 안되어있을때는 부드럽게 옮기기
		}
		else
		{
			if (Distance <= MainToLockActorDistance) // 락 대상과 메인의 거리가 일정량 이하가 되면 자연스럽게 카메라가 고정되야함
			{
				lookAtRotation.Pitch -= targetingHeightOffset;	
			}
			GetController()->SetControlRotation(lookAtRotation); // 컨트롤러만 돌리기 (카메라만)
		}

		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMovementMontage)
		{
			float FwdAxisValue = InputComponent->GetAxisValue("MoveForward");
			float RightAxisValue = InputComponent->GetAxisValue("MoveRight");
			
			if (!bAttacking)
			{
				if (FMath::Abs(FwdAxisValue) > FMath::Abs(RightAxisValue))
				{
					// 전후진 입력 우선 처리
					if (FwdAxisValue > 0.0f)
					{
						// 전진 입력 처리
						if (!AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Play(CombatMovementMontage, 0.8f);
							AnimInstance->Montage_JumpToSection(FName("Jog_Fwd"), CombatMovementMontage);
						}

					}
					else if (FwdAxisValue < 0.0f)
					{
						// 후진 입력 처리
						if (!AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Play(CombatMovementMontage, 0.8f);
							AnimInstance->Montage_JumpToSection(FName("Jog_Bwd"), CombatMovementMontage);
						}
					}
					else
					{
						// 중립 상태 처리
						if (AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Stop(0.2f, CombatMovementMontage);
						}
					}
				}
				else
				{
					// 좌우 입력 우선 처리
					if (RightAxisValue > 0.0f)
					{
						// 우측 입력 처리
						if (!AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Play(CombatMovementMontage, 0.8f);
							AnimInstance->Montage_JumpToSection(FName("Jog_Right"), CombatMovementMontage);
						}
					}
					else if (RightAxisValue < 0.0f)
					{
						// 좌측 입력 처리
						if (!AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Play(CombatMovementMontage, 0.8f);
							AnimInstance->Montage_JumpToSection(FName("Jog_Left"), CombatMovementMontage);
						}
					}
					else
					{
						// 중립 상태 처리
						if (AnimInstance->Montage_IsPlaying(CombatMovementMontage))
						{
							AnimInstance->Montage_Stop(0.2f, CombatMovementMontage);
						}
					}
				}
			}
			
		}
		
	}
	
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
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

	PlayerInputComponent->BindAction("Targeting", IE_Pressed, this, &AMain::ToggleLockOn);


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
	if (StaminaStatus == EStaminaStatus::ESS_Normal || StaminaStatus == EStaminaStatus::ESS_BelowMinimum)
	{
		bCtrlKeyDown = true;
		SetMovementStatus(EMovementStatus::EMS_Walking);
	}
	
}

void AMain::CtrlKeyUp()
{
	if (MovementStatus == EMovementStatus::EMS_Walking)
	{
		bCtrlKeyDown = false;
		SetMovementStatus(EMovementStatus::EMS_Normal);
	}
	
}

void AMain::Dashing()
{
	UE_LOG(LogTemp, Warning, TEXT("Dash!"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	class UMainAnimInstance* MainAnimInstance;
	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());
	if (MainAnimInstance != nullptr)
	{
		if ((!bDashing) && !(MainAnimInstance->bIsInAir) && (Stamina >= DashStamina) && (!bAttacking) && !bStunned) // UMainAnimInstance클래스의 bIsInAir를 써 공중에 있을때는 대쉬를 못하게 함
		{
			if (!bLockOn)
			{
				const FVector ForwardDir = this->GetActorRotation().Vector(); // 대쉬 방향 캐릭터가 보고잇는 방향
				LaunchCharacter(ForwardDir * DashDistance, true, false);

				if (AnimInstance && DashMontage)
				{
					AnimInstance->Montage_Play(DashMontage, 1);

				}
			}
			else
			{
				float FwdAxisValue = InputComponent->GetAxisValue("MoveForward");
				float RightAxisValue = InputComponent->GetAxisValue("MoveRight");

				//수정해야함 -> 캐릭터의 방향을 보는것이아니게 하든 아니면 캐릭터의 방향을 틱함수에서 똑바로 하게하든 해야함 삐뚤어져있음`
				FVector DashDirection;
				if (FMath::Abs(FwdAxisValue) > FMath::Abs(RightAxisValue))
				{
					// 전후진 입력 우선 처리
					if (FwdAxisValue > 0.0f)
					{
						// 전진 입력 처리
						DashDirection = this->GetActorRotation().RotateVector(FVector::ForwardVector);
					}
					else if (FwdAxisValue < 0.0f)
					{
						// 후진 입력 처리
						DashDirection = this->GetActorRotation().RotateVector(FVector::BackwardVector);
					}
					else
					{
						// 중립 상태 처리
						return;
					}
				}
				else
				{
					// 좌우 입력 우선 처리
					if (RightAxisValue > 0.0f)
					{
						// 우측 입력 처리
						DashDirection = this->GetActorRotation().RotateVector(FVector::RightVector);
					}
					else if (RightAxisValue < 0.0f)
					{
						// 좌측 입력 처리
						DashDirection = -1 * this->GetActorRotation().RotateVector(FVector::RightVector);
					}
					else
					{
						// 중립 상태 처리
						return;
					}
				}

				LaunchCharacter(DashDirection * DashDistance, false, false);
				
			}
			

			UGameplayStatics::PlaySound2D(this, DashSound);
			Stamina -= DashStamina;
			bDashing = true;
		}
	}
	
}

void AMain::Stunned()
{
	bReadyStunned = false;

	SetMovementStatus(EMovementStatus::EMS_Normal);
	SetMovementStatus(EMovementStatus::EMS_Stun);

	SetInterpToEnemy(false);

	UE_LOG(LogTemp, Warning, TEXT("Stun!"));

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	class UMainAnimInstance* MainAnimInstance;
	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	if (StunSound)
	{
		if (StunAudioComponent->Sound != StunSound) 
			StunAudioComponent->SetSound(StunSound);

		if (!StunAudioComponent->IsPlaying())
			StunAudioComponent->Play();

	}

	if (MainAnimInstance != nullptr)
	{
		//UGameplayStatics::PlaySound2D(this, StunSound);
		
		
		bStunned = true;

		if (AnimInstance && StunMontage)
		{
			if (bDashing || bShiftKeyDown)
			{
				AnimInstance->Montage_Play(StunMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Stun_Start"), StunMontage);
				bDashing = false;
				bShiftKeyDown = false;
			}
			else if (bAttacking)
			{
				
				if (AnimInstance->Montage_IsActive(CombatMontage))
				{
					AnimInstance->Montage_Stop(0.f,CombatMontage);
				}
				AnimInstance->Montage_Play(StunMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Stun_Idle"), StunMontage);
				bAttacking = false;

			}
				
		}
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

	if (MovementStatus == EMovementStatus::EMS_Stun ||
		CombatStatus == ECombatStatus::ECS_StunTakeDamage)
		return;
	// 클릭 횟수 증가
	ClickCount++;
	
	float CurrentTime = GetWorld()->GetTimeSeconds();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (!bDashing && !bStunned)
	{
		if (bFirstClick)
		{
			
			Attack();
			bFirstClick = false;
			//LastLeftClickTime = CurrentTime;

		}
		else if ((!bLMBDown) && !bLastAttack)
		{
			// Double click
			NextAttackAnim();
			
		}
		else
		{
			// Single click
			Attack();
		}
	}
	bLMBDown = true;
}

void AMain::CountClicked()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	const FAnimMontageInstance* CombatMontageInstance = AnimInstance->GetActiveInstanceForMontage(CombatMontage);
	if (CombatMontage && !AnimInstance->Montage_IsPlaying(CombatMontage))
	{
		//const float CurrentTime = CombatMontageInstance->GetPosition();

		// 처음 클릭 이벤트 처리
		if (!bFirstClick)
		{
			
			return;
		}
		if (ClickCount >= 2)
		{

		}

	}

}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::RMBDown()
{
	if (MainPlayerController)
		if (MainPlayerController->bPauseMenuVisible)
			return;


	if (MovementStatus == EMovementStatus::EMS_Stun ||
		CombatStatus == ECombatStatus::ECS_StunTakeDamage)//||
		//Mutant->EnemyMovementStatus != EEnemyMovementStatus::EMS_Stun)
		return;

	bRMBDown = true;
	if (!bRMBFirstClick)
	{
		bRMBFirstClick = true;
		AttackCritical();
	}
}

void AMain::RMBUp()
{
	bRMBDown = false;
	bRMBFirstClick = false;
}

void AMain::AttackCritical()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!bAttacking &&
		MovementStatus != EMovementStatus::EMS_Dead &&
		MovementStatus != EMovementStatus::EMS_Stun &&
		CombatStatus != ECombatStatus::ECS_StunTakeDamage &&
		CombatTarget !=nullptr&&
		CombatTarget->EnemyMovementStatus == EEnemyMovementStatus::EMS_CriticalStun&&
		Stamina > 0.f &&
		!bCriticalAttack)
	{
		bCriticalAttack = true;
		Stamina -= AttackStamina;
		bAttacking = true;
		SetInterpToEnemy(true);
		UE_LOG(LogTemp, Warning, TEXT("Critical Attack!!"));
		if (AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.f);
			AnimInstance->Montage_JumpToSection(FName("CriticalAttack2"), CombatMontage);
		}

	}
}

void AMain::AttackCriticalDamage()
{
	
}

void AMain::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!!"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!bAttacking && 
		MovementStatus != EMovementStatus::EMS_Dead && 
		MovementStatus != EMovementStatus::EMS_Stun &&
		CombatStatus != ECombatStatus::ECS_StunTakeDamage &&
		Stamina > 0.f )
	{
		Stamina -= AttackStamina; //공격시 스태미너 AttackStamina만큼 - 하기
		bAttacking = true;

		if(!bLockOn)
			SetInterpToEnemy(true);

		
		UE_LOG(LogTemp, Warning, TEXT("CurrentComnoCount : %d"), CurrentComboCount);
		
	
		//애니메이션 몽타주 재생하는법
		
		if (AnimInstance && CombatMontage)
		{
			const FVector ForwardDir = this->GetActorRotation().Vector(); // 캐릭터가 가고있는 앞쪽방향
			if (CurrentComboCount == 0)
			{
				
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);

				LaunchCharacter(ForwardDir * AttackDistance, true, false);  //AttackDistance만큼 앞으로


			}
			else if (CurrentComboCount == 1)
			{
				
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
				LaunchCharacter(ForwardDir * AttackDistance, true, false);
				
			}
			else if (CurrentComboCount == 2)
			{
				
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);
				LaunchCharacter(ForwardDir * AttackDistance, true, false);
				
				bLastAttack = true;
			}
		}
	}

	
	/* 현재 캐릭터가 바라보는 방향으로 회전
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector DirectionY1 = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //오른쪽
	const FVector DirectionY2 = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //왼쪽

	const FVector DirectionX1 = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //앞
	const FVector DirectionX2 = -FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);  //뒤

	if(MainPlayerController->IsInputKeyDown(EKeys::D))
	{
		SetActorRotation(DirectionY1.Rotation());
	}
	else if (MainPlayerController->IsInputKeyDown(EKeys::A))
	{
		SetActorRotation(DirectionY2.Rotation());
	}
	if (MainPlayerController->IsInputKeyDown(EKeys::W))
	{
		SetActorRotation(DirectionX1.Rotation());
	}
	else if (MainPlayerController->IsInputKeyDown(EKeys::S))
	{
		SetActorRotation(DirectionX2.Rotation());
	}
	*/
}

void AMain::AttackEnd()
{

	UE_LOG(LogTemp, Warning, TEXT("Attack End!!"));
	bAttacking = false;
	SetInterpToEnemy(false);
	bFirstClick = true;
	bLastAttack = false;
	CurrentComboCount = 0;

	
	//if (bLMBDown)
	//{
	//	Attack();
	//}
	
}

void AMain::StaminaDelay(float Time)
{
	float DeltaStamina = StaminaDrainRate * Time;
		
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (Stamina - DeltaStamina <= MinSprintStamina) //Stamina가 MinSprintStamina보다 아래면 BelowMinimum으로 상태가 변함
		{
			SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
		}
	case EStaminaStatus::ESS_BelowMinimum:
		if (Stamina - DeltaStamina <= 0.f)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
			Stamina = 0.f;
			
			Stunned();
		}
		break;
	default:
		break;
	}
	GetWorld()->GetTimerManager().SetTimer(StaminaDelayTimer,
		FTimerDelegate::CreateUObject(this, &AMain::RcoveringStamina, Time),
		AttackReCoverStaminaTime, false, AttackReCoverStaminaTime);
	ShiftKeyUp(); // shift키를 누르면서 공격중이어도 안눌린 것처럼 만들어줌
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

void AMain::DecrementHealth(float Amount)
{
	/*
	if (Health - Damage <= 0.f)
	{
		Health -= Damage;
		Die();
	}
	else
	{
		Health -= Damage;
	}
	*/
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		const FVector EnemyForwardDir = CombatTarget->GetActorRotation().Vector(); // 에너미가 가고있는 앞쪽방향
		if (MovementStatus == EMovementStatus::EMS_Stun)
		{
			SetCombatStatus(ECombatStatus::ECS_StunTakeDamage);
			LaunchCharacter(EnemyForwardDir * attackedDistance * 3, true, false);  //AttackDistance만큼 앞으로
			AnimInstance->Montage_Play(CombatMontage, 0.9f);
			AnimInstance->Montage_JumpToSection(FName("CriticalHitReact"), CombatMontage);
			UGameplayStatics::PlaySound2D(this, CriticalDamagedSound);
			bStunned = false;
			bReadyStunned = true;

		}
		else
		{
			if (!bDashing && !bStunned )//&&!bAttacking)
			{
				SetCombatStatus(ECombatStatus::ECS_TakeDamage);
				LaunchCharacter(EnemyForwardDir * attackedDistance, true, false); 
				AnimInstance->Montage_Play(CombatMontage, 1.f);
				AnimInstance->Montage_JumpToSection(FName("HitReact"), CombatMontage);
				UGameplayStatics::PlaySound2D(this, DamagedSound);
				AttackEnd();
			}

			
		}
	}
	

	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AMutant* Enemy = Cast <AMutant>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		/*
		if(MovementStatus == EMovementStatus::
		_Stun)
			Health -= CombatTarget->D;
		else
		*/
		Health -= DamageAmount;
	}


	
	return DamageAmount;
}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));

	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
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

	if (!this->GetMovementComponent()->IsFalling())
	{
		
		UGameplayStatics::PlaySound2D(this, JumpSound);

	}
}

void AMain::RcoveringStamina(float Time)
{
	
	//float DeltaStamina = StaminaDrainRate * GetWorld()->DeltaTimeSeconds;
	float DeltaStamina = StaminaDrainRate * Time;
	
	//UE_LOG(LogTemp, Warning, TEXT("%f"),Stamina);
	
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if ((bShiftKeyDown || bDashing) && !bCtrlKeyDown && !bStunned) // 달리거나 대쉬할때, 걷지않을때 
		{
			//StaminaDelay(Time);

			if (Stamina - DeltaStamina <= MinSprintStamina) //Stamina가 MinSprintStamina보다 아래면 BelowMinimum으로 상태가 변함
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				Stamina -= DeltaStamina;
			}
			else
			{
				Stamina -= DeltaStamina;
			}

			if (bMovingForward || bMovingRight)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
			else
			{
				if (CombatStatus != ECombatStatus::ECS_StunTakeDamage)
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				
			}
		}
		else if (!bShiftKeyDown || !bDashing || bCtrlKeyDown) //shift Key up 혹은 대쉬 안할때
		{
			if (Stamina + DeltaStamina >= MaxStamina)
			{
				Stamina = MaxStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			if (CombatStatus != ECombatStatus::ECS_StunTakeDamage)
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}

			if (bCtrlKeyDown)
				SetMovementStatus(EMovementStatus::EMS_Walking);
		}
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if ((bShiftKeyDown || bDashing ) && !bCtrlKeyDown && !bStunned)
		{
			if (Stamina - DeltaStamina <= 0.f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				Stamina = 0;
				
			}
			else
			{
				Stamina -= DeltaStamina;
				if (bMovingForward || bMovingRight)
				{
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
			}

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
		if ((bShiftKeyDown || bDashing ) && !bCtrlKeyDown)
		{
			Stamina = 0.f;
			
			Stunned();
		}
		else if (!bShiftKeyDown || !bDashing || bCtrlKeyDown)// shift Key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			Stamina += DeltaStamina;
		}

		if (!bCtrlKeyDown)
			SetMovementStatus(EMovementStatus::EMS_Stun);
		else
			SetMovementStatus(EMovementStatus::EMS_Walking);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			if (CombatStatus != ECombatStatus::ECS_StunTakeDamage)
			{
				SetCombatStatus(ECombatStatus::ECS_Normal);
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}	
			Stamina += DeltaStamina;

		}
		else
		{
			//SetCombatStatus(ECombatStatus::ECS_Stun);
			Stamina += DeltaStamina;
		}

		/* 어차피 스턴 상태라 못움직임
		if (!bCtrlKeyDown)
			SetCombatStatus(ECombatStatus::ECS_Stun);
		else
			SetMovementStatus(EMovementStatus::EMS_Walking);
		*/
		break;
	default:
		;

	}
}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return (Value != 0.0f) &&
			 (!bDashing) &&
			(!bAttacking) &&
			MovementStatus != EMovementStatus::EMS_Stun &&
			CombatStatus != ECombatStatus::ECS_StunTakeDamage&&
			CombatStatus != ECombatStatus::ECS_TakeDamage &&
			(!bStunned)&&
			!MainPlayerController->bPauseMenuVisible;
	}
	return false;
}

void AMain::MoveForward(float Value)
{
	bMovingForward = false;
	if (CanMove(Value))
	{
		//앞으로 가는 방향 설정 (const는 바꾸지 않을 것에 사용)(FRotator, FVector 앞에 const가 다 있었음)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		AddMovementInput(Direction, Value);

		bMovingForward = true;
	}
}

void AMain::MoveRight(float Value)
{
	bMovingRight = false;
	
	if (CanMove(Value))
	{
		//옆으로 가는 방향 설정 (const는 바꾸지 않을 것에 사용)
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(Direction, Value);

		bMovingRight = true;
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
					//actor(MyProject의Weapon.cpp)는 Skeletalmesh를 받고 char는 GetMesh()
					FVector SocketLocation = WeaponSocket->GetSocketLocation(GetMesh()); 
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Enemy->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
			}

			
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Enemy, HealthDamage, (CriticalHealthDamage, StaminaDamage, MaxStaminaDamage, WeaponInstigator), this, DamageTypeClass);
			}
		}
	}
}

void AMain::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AMain::ActivateCollision()
{
	SwordCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMain::DeactivateCollision()
{
	SwordCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMain::ToggleLockOn()
{
	
	if (MainPlayerController)
	{
		
		if (bTargetingBoxOverlap)
		{
			MainPlayerController->ToggleTargetingCrossHair();
			if (bLockOn)
			{
				SetCombatStatus(ECombatStatus::ECS_Normal);
				bLockOn = false;
				lockedOnActor = nullptr;
			}
			else
			{
				SetCombatStatus(ECombatStatus::ECS_Targeting);

				if (lockOnCandidates.Num() > 0)
				{
					lockedOnActor = lockOnCandidates[0];
					if (lockedOnActor)
					{
						bLockOn = true;
					}
				}
			}
		}
		
	}

}

void AMain::TargetingBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMutant* Mutant = Cast<AMutant>(OtherActor);

		if (Mutant)
		{
			bTargetingBoxOverlap = true;
			lockOnCandidates.AddUnique(Mutant);

			SetHasCombatTarget(true);
			SetCombatTarget(Mutant);

			// 현재 잡히는 뮤턴트의 개수 출력
			UE_LOG(LogTemp, Warning, TEXT("current Mutant: %d"), lockOnCandidates.Num());

		}
	}
}

void AMain::TargetingBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	if (OtherActor)
	{
		AMutant* Mutant = Cast<AMutant>(OtherActor);
		if (Mutant) 
		{
			bTargetingBoxOverlap = false;
			lockOnCandidates.Remove(Mutant);

			if (!Mutant->bAgroShpehreOverlap)//뮤턴트의 어그로 스피어 밖일때
			{
				//락온을 강제해제 시킴
				SetHasCombatTarget(false);
				SetCombatTarget(nullptr);

				Mutant->CombatTarget = nullptr;

				if (MainPlayerController)
				{
					MainPlayerController->RemoveTargetingCrossHair();
				}
				bLockOn = false;

			}
			
			
		}
	}
}

/*
//카메라에 부착된 박스 콜리전
void AMain::TargetingCameraBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor)
	{
		AMutant* Mutant = Cast<AMutant>(OtherActor);

		if (Mutant)
		{
			bTargetingCameraBoxOverlap = true;
			SetHasCombatTarget(true);
			SetCombatTarget(Mutant);

			// 현재 잡히는 뮤턴트의 개수 출력
			UE_LOG(LogTemp, Warning, TEXT("현재 잡히는 뮤턴트의 개수: %d"), lockOnCandidates.Num());

			lockOnCandidates.AddUnique(Mutant);
		}
	}
}

void AMain::TargetingCameraBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMutant* Mutant = Cast<AMutant>(OtherActor);
		if (Mutant && !Mutant->bAgroShpehreOverlap)
		{

			bTargetingCameraBoxOverlap = false;
			SetHasCombatTarget(false);
			SetCombatTarget(nullptr);

		}
	}
}
*/