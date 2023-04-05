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
	void StopJump();

	//구르기, 대쉬
	void Dash();
	void StopDash();

	//타겟팅
	void TargetingMode();
	void TravelMode();

	//공격 받았을시
	void DecrementHealth(float Damage);

	//공격
	void Attacking();

};
