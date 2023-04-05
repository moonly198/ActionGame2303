// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

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

	/*
	PlayerStat
	*/

	UPROPERTY(EditAnywhere, Category = Stats)
		float MaxHealth = 1000;

	UPROPERTY(EditAnywhere, Category = Stats)
		float Health = 800;

	UPROPERTY(EditAnywhere, Category = Stats)
		float MaxStamina = 1000;

	UPROPERTY(EditAnywhere, Category = Stats)
		float Stamina = 800;

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
	void StopJump();

	//������, �뽬
	void Dash();
	void StopDash();

	//Ÿ����
	void TargetingMode();
	void TravelMode();

	//���� �޾�����
	void DecrementHealth(float Damage);

	//����
	void Attacking();

};
