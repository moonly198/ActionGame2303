// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Dummy.generated.h"

UENUM(BlueprintType)
enum class EDummyMovementStatus :uint8
{

	DMS_Idle			UMETA(DeplayName = "Idle"),
	DMS_MoveToTarget	UMETA(DeplayName = "MoveToTarget"),
	DMS_Attacking		UMETA(DeplayName = "Attacking"),
	DMS_Beaten		UMETA(DeplayName = "Beaten"),
	DMS_Stun		UMETA(DeplayName = "Stun"),
	DMS_CriticalStun		UMETA(DeplayName = "CriticalStun"),
	DMS_Dead		UMETA(DeplayName = "Dead"),

	DMS_MAX				UMETA(DeplayName = "DefaultMax")

};


UCLASS()
class ACTIONGAME2303_API ADummy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADummy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		EDummyMovementStatus DummyMovementStatus;

	FORCEINLINE void SetDummyMovementStatus(EDummyMovementStatus Status) { DummyMovementStatus = Status; }
	FORCEINLINE EDummyMovementStatus GetEnemyMovementStatus() { return DummyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		class USphereComponent* AgroSphere;


	//스테이터스
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

	//공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class UParticleSystem* HitParticles; //맞을때 파티클

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float EnemyHealthDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
		float EnemyCriticalDamage;

	bool bTakeDamage = false;

	//스턴
	void Stunned();

	UPROPERTY(VisibleAnywhere)
		bool bStunned;

	bool bWasStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		float StunTime = 3.f;

	FTimerHandle StunTimer;

	void CriticalStunned();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stun")
		bool bCriticalStunned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stun")
		float CriticalStunTime = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
		TSubclassOf<UDamageType>DamageTypeClass;



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


	UFUNCTION(BlueprintCallable)
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
