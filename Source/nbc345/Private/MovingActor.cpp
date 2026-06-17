#include "MovingActor.h"

AMovingActor::AMovingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MoveSpeed = 200.f;
	MaxRange = 500.f;
	MoveDirection = FVector(1.f, 0.f, 0.f);
}

void AMovingActor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	MoveDirection.Normalize();
}

void AMovingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	FVector Offset = CurrentLocation - StartLocation;

	float DistanceOnLine = FVector::DotProduct(Offset, MoveDirection);

	if (DistanceOnLine >= MaxRange)
	{
		MoveSpeed = -FMath::Abs(MoveSpeed);
		CurrentLocation = StartLocation + (MoveDirection * MaxRange);
	}
	else if (DistanceOnLine <= 0.f)
	{
		MoveSpeed = FMath::Abs(MoveSpeed);
		CurrentLocation = StartLocation;
	}

	CurrentLocation += MoveDirection * MoveSpeed * DeltaTime;
	SetActorLocation(CurrentLocation);
}