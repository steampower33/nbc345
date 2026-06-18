// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h" // Enhanced Input Value 구조체
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
    virtual void BeginPlay() override;

public:	
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    /* =========================================================
     * 컴포넌트 계층 구조 (Component Hierarchy)
     * ========================================================= */
    // 루트 충돌체
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCapsuleComponent> CapsuleComp;

    // 캐릭터 메쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USkeletalMeshComponent> MeshComp;

    // 카메라 지지대
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USpringArmComponent> SpringArmComp;

    // 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UCameraComponent> CameraComp;

    /* =========================================================
     * Enhanced Input 시스템 데이터
     * ========================================================= */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> IA_Look;

    /* =========================================================
     * 이동 및 회전 제어 변수
     * ========================================================= */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float TurnSpeed;

    // 바인딩될 입력 처리 함수들
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
};
