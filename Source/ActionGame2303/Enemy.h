// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus :uint8
{

	EMS_Idle			UMETA(DeplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DeplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DeplayName = "Attacking"),
	EMS_Beaten		UMETA(DeplayName = "Beaten"),
	EMS_Stun		UMETA(DeplayName = "Stun"),
	EMS_CriticalStun		UMETA(DeplayName = "CriticalStun"),
	EMS_Dead		UMETA(DeplayName = "Dead"),

	EMS_MAX				UMETA(DeplayName = "DefaultMax")

};


UCLASS()
class ACTIONGAME2303_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	//공격시 에너미 쪽을 향해 캐릭터 회전
	float InterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		//class AMain* CombatTarget;

	FORCEINLINE void SetCombatTarget(AMain* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);


	//class AMain* Main;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		bool bHasValidTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class AAIController* AIController;

	//체력 스태미나 스탯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float CurrentMaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float EnemyHealthDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float EnemyCriticalDamage;

	//공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UParticleSystem* HitParticles; //맞을때 파티클

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		USoundCue* SwingSound;


	//컴뱃 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UAnimMontage* CriticalCombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bCriticalAttack = false;

	bool bTakeDamage = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		bool bBeaten;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackedDistance;

	//Stun
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stun")
		UAnimMontage* StunMontage;

	virtual void Stunned();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		USoundCue* StunSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		float StunTime;

	UPROPERTY(VisibleAnywhere)
		bool bStunned;

	bool bWasStunned = false;

	//CriticalStun
	virtual void CriticalStunned();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		USoundCue* CriticalStunSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		float CriticalStunTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stun")
		bool bCriticalStunned;

	UPROPERTY(VisibleAnywhere)
		bool bCriticalStunnedAnimPlay;

	FTimerHandle StunTimer;

	// enemy가 시간을 두고 공격하기 원함
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackMinTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType>DamageTypeClass;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float DeathDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void MoveToTarget(class AMain* Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		AMain* CombatTarget;

	UFUNCTION()
		virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		virtual void ActivateCollision();

	UFUNCTION(BlueprintCallable)
		virtual void DeactivateCollision();


	UFUNCTION(BlueprintCallable)
		virtual void Attack();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		bool bAttacking;

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	UFUNCTION(BlueprintCallable)
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		void DeathEnd();

	UFUNCTION(BlueprintCallable)
		bool Alive();

	virtual void Die();

	void Disappear();

};
