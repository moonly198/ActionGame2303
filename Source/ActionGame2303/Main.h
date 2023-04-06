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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
		class AMainPlayerController* MainPlayerController;

	//�÷��̾� �ڿ� ��ġ�� ī�޶� �Ÿ� ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	//����ٴϴ� ī�޶�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

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

	bool bShiftKeyDown;

	void ShiftKeyDown();

	void ShiftKeyUp();
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

	//�̵�
	void MoveForward(float Value);
	void MoveRight(float Value);

	//������ ������(���� ������)
	void Turn(float Value);
	//������ ������(�� �Ʒ�)
	void LookUp(float Value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//����
	void Jump();
	

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

	UPROPERTY(EditAnywhere)
		UAnimMontage* DashMontage;




	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bRMBDown;
	void RMBDown();
	void RMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();


	//Ÿ����
	void TargetingMode();
	void TravelMode();

	//���� �޾�����
	void DecrementHealth(float Damage);
	void Die();

	//����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
		bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
		class UAnimMontage* CombatMontage;



};
