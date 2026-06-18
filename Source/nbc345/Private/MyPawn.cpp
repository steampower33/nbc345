#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AMyPawn::AMyPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    // 캡슐 컴포넌트 생성 및 루트 설정
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    RootComponent = CapsuleComp;
    CapsuleComp->SetSimulatePhysics(false); // 물리 대신 코드로 직접 제어

    // 스켈레탈 메쉬 컴포넌트 부착
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetSimulatePhysics(false);

    // 스프링암 컴포넌트 부착
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->TargetArmLength = 400.0f;
    // 컨트롤러 회전을 사용하지 않으므로 강제 상속 해제 (우리가 직접 SpringArm을 돌릴 것임)
    SpringArmComp->bUsePawnControlRotation = false; 

    // 카메라 컴포넌트 부착
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    // 기본 속도 세팅
    MoveSpeed = 600.f;
    TurnSpeed = 100.f;
}

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    // Enhanced Input System의 LocalPlayer Subsystem에 Mapping Context 등록
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced Input Component로 캐스팅하여 바인딩
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Move (Triggered: 키를 누르고 있는 동안 매 프레임 호출됨)
        if (IA_Move)
        {
            EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyPawn::Move);
        }

        // Look (Triggered)
        if (IA_Look)
        {
            EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyPawn::Look);
        }
    }
}

void AMyPawn::Move(const FInputActionValue& Value)
{
    // 입력값(Vector2D) 가져오기
    FVector2D MovementVector = Value.Get<FInputActionValue::Axis2D>();

    if (Controller != nullptr)
    {
        // 프레임 독립성을 보장하기 위한 DeltaTime 확보
        float DeltaTime = GetWorld()->GetDeltaSeconds();

        // AddActorLocalOffset은 로컬 좌표계를 기준으로 이동한다.
        // X축(Forward)은 W/S, Y축(Right)은 A/D에 대응되도록 벡터를 구성한다.
        FVector LocalOffset = FVector(MovementVector.Y, MovementVector.X, 0.f);
        
        // 이동 거리 = 방향 * 속도 * 시간
        LocalOffset = LocalOffset * MoveSpeed * DeltaTime;

        // true(bSweep)로 설정하여 이동 중 콜리전에 부딪히면 막히도록 처리
        AddActorLocalOffset(LocalOffset, true);
    }
}

void AMyPawn::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FInputActionValue::Axis2D>();

    if (Controller != nullptr)
    {
        float DeltaTime = GetWorld()->GetDeltaSeconds();

        // 좌우(Yaw) 회전 - Pawn 전체(Actor)를 로컬 축 기준으로 회전시킨다.
        float YawDelta = LookAxisVector.X * TurnSpeed * DeltaTime;
        AddActorLocalRotation(FRotator(0.f, YawDelta, 0.f));

        // 상하(Pitch) 회전 - 카메라를 담고 있는 SpringArm만 회전시킨다 (Actor가 뒤집어지는 것 방지).
        // 마우스 위로(Y+) 올릴 때 위를 보게 하려면 부호에 주의. (일반적으로 상하 반전 방지를 위해 -를 곱하거나 세팅에서 조절)
        float PitchDelta = LookAxisVector.Y * TurnSpeed * DeltaTime;
        
        // 현재 스프링암의 로컬 회전값을 가져와서 Pitch를 더해줌
        FRotator CurrentSpringArmRot = SpringArmComp->GetRelativeRotation();
        float NewPitch = FMath::Clamp(CurrentSpringArmRot.Pitch + PitchDelta, -80.0f, 80.0f); // 짐벌락 방지용 클램프
        
        CurrentSpringArmRot.Pitch = NewPitch;
        SpringArmComp->SetRelativeRotation(CurrentSpringArmRot);
    }
}