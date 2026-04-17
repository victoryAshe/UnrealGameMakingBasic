// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


AABCharacterPlayer::AABCharacterPlayer()
{
	// 기본 설정.
	// Controller 회전 값을 받아서 설정하는 옵션 비활성화.
	bUseControllerRotationPitch = false;	// Y축 회전.
	bUseControllerRotationYaw = false;		// Z축 회전.
	bUseControllerRotationRoll = false;		// X축 회전.

	// Movement Component 설정.
	// Character가 이동하는 방향에 맞게 부드럽게 회전을 하도록 하는 옵션.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 얼마나 부드럽게 할 것인가. // Y, Z, X
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	// MeshComponent 설정.
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -88.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	// Mesh Asset 지정 (검색 필요)
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple")
	);

	if (CharacterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	// Anim Blueprint Class 정보 지정.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed.ABP_Unarmed_C")
	);

	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}


	// Create Components.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArm"));

	// 계층 설정 (Root Component 아래로).
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 600.0f; // 6m.

	// Controller Rotaion 값을 사용할 지 여부.
	SpringArm->bUsePawnControlRotation = true;

	// Camera Component.
	Camera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("Camera"));

	Camera->SetupAttachment(SpringArm);

	// 입력 관련 설정.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> 
		DefaultMappingContextRef(
		TEXT("/Game/Input/IMC_Default.IMC_Default")
	);

	if (DefaultMappingContextRef.Succeeded())
	{
		DefaultMappingContext = DefaultMappingContextRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		MoveActionRef(
			TEXT("/Game/Input/Actions/IA_Move.IA_Move")
		);

	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		LookActionRef(
			TEXT("/Game/Input/Actions/IA_Look.IA_Look")
		);

	if (LookActionRef.Succeeded())
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		JumpActionRef(
			TEXT("/Game/Input/Actions/IA_Jump.IA_Jump")
		);

	if (JumpActionRef.Succeeded())
	{
		JumpAction = JumpActionRef.Object;
	}
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 사용할 IMC 지정.
	// DownCasting.
	APlayerController* PlayerController
		= Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// Enhanced Input System의 Sub System 가져오기.
		UEnhancedInputLocalPlayerSubsystem* InputSystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()
		);

		// EnhancedInputSubSystem 얻어온 후에, 사용할 MappingContext 설정.
		if (InputSystem)
		{
			InputSystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Binding: Enhanced Input System Component를 활용해서 설정.
	UEnhancedInputComponent* EnhancedInputComponent
		= Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		// Input Binding: Event와 실행 함수를 연결하는 과정.
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&AABCharacterPlayer::Move
		);

		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Started,
			this,
			&AABCharacterPlayer::Look
		);

		// Jump는 두 개의 함수를 Bind 해야 함!
		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Started,
			this,
			&ACharacter::Jump
		);

		EnhancedInputComponent->BindAction(
			JumpAction,
			ETriggerEvent::Completed,
			this,
			&ACharacter::StopJumping
		);
	}
}

void AABCharacterPlayer::Move(const FInputActionValue& Value)
{
	// 입력 값 읽어오기 (입력에 지정된 Type으로 변환).
	FVector2D Movement = Value.Get<FVector2D>();

	// 이동할 방향 만들기.
	// Camera가 바라보는 방향 (Controller의 방향)을 기준으로.
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	// 오일러 기준으로 회전 행렬 만들기 <=> 쿼터니언.
	// 앞 방향.
	FVector ForwardVector
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// 오른쪽 방향.
	FVector RightVector
		= FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// Movement Component에 Input 전달.
	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);
}

void AABCharacterPlayer::Look(const FInputActionValue& Value)
{

}
