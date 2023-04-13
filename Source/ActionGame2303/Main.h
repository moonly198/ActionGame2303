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

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SKeletalMesh")
	//	class USkeletalMeshComponent* SkeletalMesh;

	//무기관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class USoundCue* HitSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class USoundCue* SwordSwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		USoundCue* CharSwingSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* JumpSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundCue* DashSound;

	//공격시 에너미 쪽을 향해 캐릭터 회전
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		class AMutant* CombatTarget;

	FORCEINLINE void SetCombatTarget(AMutant* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	

	//플레이어 뒤에 위치한 카메라 거리 빨간줄
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	//따라다니는 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	//카메라 회전기능 회전율
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	//ENUM Status BlueprintReadWrite
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
		EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus status) { StaminaStatus = status; }

	//스태미너 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float StaminaDrainRate;

	//스태미너 지치는 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float MinSprintStamina;

	// 움직임 상태 설정과 달리는 속도
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
		float WalkingSpeed;

	bool bShiftKeyDown;
	void ShiftKeyDown();
	void ShiftKeyUp();

	bool bCtrlKeyDown;
	void CtrlKeyDown();
	void CtrlKeyUp();
	/*
	PlayerStat
	*/

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Player Stats")
		float MaxHealth = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float Health = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float MaxStamina = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float Stamina = 800.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//이동
	void MoveForward(float Value);
	void MoveRight(float Value);

	//가로축 돌리기(왼쪽 오른쪽)
	void Turn(float Value);
	//세로축 돌리기(위 아래)
	void LookUp(float Value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//점프
	void Jump();

	//구르기, 대쉬
	void Dashing();
	UPROPERTY(EditAnywhere)
	float DashDistance = 6000.f;
	
	UPROPERTY(EditAnywhere)
	float DashStamina = 150.f;

	UPROPERTY(VisibleAnywhere)
		bool bDashing;

	UPROPERTY(VisibleAnywhere)
		bool bPlayingDashMontage;

	UPROPERTY(EditAnywhere)
		UAnimMontage* DashMontage;



	UPROPERTY(VisibleAnywhere, Category = "Combat")
	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	float LastLeftClickTime;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ClickInterval = 1.f;

	bool bFirstClick = true;
	bool LastAttack = false;

	
	UFUNCTION(BlueprintCallable)
		void PlaySwingSound();
	

	bool bRMBDown;
	void RMBDown();
	void RMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();


	//타겟팅
	void TargetingMode();
	void TravelMode();

	//공격 받았을시
	void DecrementHealth(float Damage);
	void Die();

	//공격
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UFUNCTION(BlueprintCallable)
		void NextAttackAnim();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		int CurrentComboCount = 0;

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




};
