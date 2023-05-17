// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Walking UMETA(DisplayName = "Walking"),
	EMS_Stun UMETA(DisplayName = "Stun"),
	//EMS_Attacking UMETA(DisplayName = "Attacking"),
	//EMS_StunTakeDamage UMETA(DisplayName = "StunTakeDamage"),
	//EMS_TakeDamage UMETA(DisplayName = "TakeDamage"),
	EMS_Dead UMETA(DisplayName = "Dead"),


	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ECombatStatus : uint8
{
	ECS_Normal UMETA(DisplayName = "Normal"),
	ECS_Attacking UMETA(DisplayName = "Attacking"),
	ECS_Targeting UMETA(DisplayName = "Targeting"),
	ECS_StunTakeDamage UMETA(DisplayName = "StunTakeDamage"),
	ECS_TakeDamage UMETA(DisplayName = "TakeDamage"),


	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")

};

UCLASS()
class ACTIONGAME2303_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
		FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
		class AMainPlayerController* MainPlayerController;

	//�������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class UBoxComponent* SwordCombatCollision;


	//���ݰ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float HealthDamage; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float CriticalHealthDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float StaminaDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float MaxStaminaDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class USoundCue* SwordSwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		USoundCue* CharSwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* DashSound;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	//	USoundCue* StunSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		class USoundBase* StunSound;  

	//AudioComponent�� ������ CreateDefaultSubobject�� �ٿ������
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sound")
	class UAudioComponent* StunAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* DamagedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* CriticalDamagedSound;

	//���ݽ� ���ʹ� ���� ���� ĳ���� ȸ��
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	FRotator GetLookAtRotationYaw(FVector Target);

	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class AMutant* CombatTarget;

	FORCEINLINE void SetCombatTarget(AMutant* Target) { CombatTarget = Target; }
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class ACharacter* CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		TArray<ACharacter*> CombatTargetList;
	
	FORCEINLINE void SetCombatTarget(ACharacter* Target) { CombatTarget = Target; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class AMutant* Mutant;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class ADummy* Dummy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") // �ӽ�
		int a;

	

	//�÷��̾� �ڿ� ��ġ�� ī�޶� �Ÿ� ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	//����ٴϴ� ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* CameraSceneComponent;

	//ī�޶� ȸ����� ȸ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	//ENUM Status BlueprintReadWrite
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EStaminaStatus StaminaStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		ECombatStatus CombatStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus status) { StaminaStatus = status; }

	//���¹̳� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float StaminaDrainRate;

	//���¹̳� ��ġ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MinSprintStamina;

	// ������ ���� ������ �޸��� �ӵ�
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float WalkingSpeed;

	UPROPERTY(VisibleAnywhere)
	bool bShiftKeyDown;

	void ShiftKeyDown();
	void ShiftKeyUp();

	UPROPERTY(VisibleAnywhere)
	bool bCtrlKeyDown;

	void CtrlKeyDown();
	void CtrlKeyUp();
	/*
	PlayerStat
	*/

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Player Stats")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float Stamina;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//�̵�
	void MoveForward(float Value);
	void MoveRight(float Value);

	bool bMovingForward;
	bool bMovingRight;

	//������ ������(���� ������)
	void Turn(float Value);
	//������ ������(�� �Ʒ�)
	void LookUp(float Value);

	bool CanMove(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//���¹̳� ȸ���Լ�
	void RcoveringStamina(float Time);

	void Stunned();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		UAnimMontage* StunMontage;

	UPROPERTY(VisibleAnywhere)
		bool bStunned;

	UPROPERTY(VisibleAnywhere)
		bool bReadyStunned;



	//����
	virtual void Jump() override;

	//������, �뽬
	void Dashing();

	UPROPERTY(EditAnywhere)
	float DashDistance = 6000.f;
	
	UPROPERTY(EditAnywhere)
	float DashStamina = 150.f;

	UPROPERTY(VisibleAnywhere)
		bool bDashing;

	UPROPERTY(VisibleAnywhere)
		bool bPlayingDashMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		UAnimMontage* DashMontage;



	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bLMBDown;
	void LMBDown();
	void LMBUp();
	void CountClicked();

	UPROPERTY(VisibleAnywhere, Category = "Combat")
		int ClickCount =0;

	float CurrentMontageTime;
	float LastClickTime;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float LastLeftClickTime;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ClickInterval = 1.f;

	
	bool bFirstClick = true;

	bool bLastAttack = false;

	bool bRMBDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bRMBFirstClick = false;
	void RMBDown();
	void RMBUp();


	bool bESCDown;
	void ESCDown();
	void ESCUp();


	//Ÿ����
	void TargetingMode();
	void TravelMode();

	//���� �޾�����
	FORCEINLINE void SetCombatStatus(ECombatStatus status) { CombatStatus = status; }
	void DecrementHealth(float Amount);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Die();

	UFUNCTION(BlueprintCallable)
		void DeathEnd();

	UPROPERTY(EditAnywhere, Category = "Combat")
		float attackedDistance = 1000.f;


	//����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	void Attack();

	void AttackCritical();

	UFUNCTION()
		void AttackCriticalDamage();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		bool bCriticalAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCriticalAttackOnce = false;

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UFUNCTION(BlueprintCallable)
		void NextAttackAnim();

	UFUNCTION(BlueprintCallable)
		void PlaySwingSound();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		int CurrentComboCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackReCoverStaminaTime;

	FTimerHandle StaminaDelayTimer;

	UPROPERTY(EditAnywhere)
		float AttackDistance = 500.f;

	UPROPERTY(EditAnywhere)
		float AttackStamina= 100.f;

	void StaminaDelay(float Time);

	//FTimerManager StaminaDelayTimer;

	//bool bMontageEnd;
	//float MontageEnd;


	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void ActivateCollision();

	UFUNCTION(BlueprintCallable)
		void DeactivateCollision();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		AController* WeaponInstigator;

	//WeaponInstigator AController�� ����ġ�� ���� ������ ���� �����ϰ���
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }

	//Ÿ���� �ý���
	void ToggleLockOn();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
		bool bLockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
		bool bWasLockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
	bool bInterpToTargetRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
	float targetingHeightOffset; // Ÿ���� ������ ī�޶� ������

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		float stopAngle; // Ÿ���� ������ ī�޶� ���� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		ACharacter* lockedOnActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
		TArray<ACharacter*> lockOnCandidates;

	class APlayerCameraManager* CameraManager;



	UFUNCTION()
		void TargetingBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void TargetingBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
		UBoxComponent* TargetingBoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
	bool bTargetingBoxOverlap;

	//Ÿ���ý� ī�޶� ���ư��� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float TargetingCameraInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LockOn")
	float MainToLockActorDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LockOn")
		UAnimMontage* CombatMovementMontage;


	void SwitchLevel(FName LevelName);

	/*
	UFUNCTION()
		void TargetingCameraBoxOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void TargetingCameraBoxOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LockOn")
		UBoxComponent* TargetingCameraBoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LockOn")
		bool bTargetingCameraBoxOverlap;
	*/
};
