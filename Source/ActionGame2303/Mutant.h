// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Mutant.generated.h"

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
class ACTIONGAME2303_API AMutant : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMutant();

	
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

	bool bHasValidTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status) { EnemyMovementStatus = Status; }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class AAIController* AIController;

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

	//공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		USoundCue* SwingSound;

	//각각 공격 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		USoundCue* SwipingAttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		USoundCue* PunchAttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		USoundCue* FootAttackSound;

	//각각 공격 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		class UBoxComponent* LeftCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		UBoxComponent* RightCombatCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
		UBoxComponent* FootCombatCollision;

	//각 공격시 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		UParticleSystem* FootHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		class UAnimMontage* CombatMontage;

	bool bTakeDamage = false;

	//맞으면 경직
	void Rigid();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bBeaten;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float AttackedDistance;

	//Stun
	void Stunned();

	void CriticalStunned();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
		UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		USoundCue* StunSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		float StunTime;

	UPROPERTY(VisibleAnywhere)
		bool bStunned;

	bool bWasStunned = false;

	UPROPERTY(VisibleAnywhere)
		bool bCriticalStunned;

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
		virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
		void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void ActivateCollision();

	UFUNCTION(BlueprintCallable)
		void DeactivateCollision();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
		bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
		void AttackEnd();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void DamagedStamina();
	void Die();

	UFUNCTION(BlueprintCallable)
		void DeathEnd();

	UFUNCTION(BlueprintCallable)
		bool Alive();

	void Disappear();


};
