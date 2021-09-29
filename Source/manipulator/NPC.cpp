// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"
#include "HAL/IConsoleManager.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ANPC::ANPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//Attach Components

	ECapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ECapsule"));
	ECapsuleComponent->SetupAttachment(RootComponent);

	ESkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ESkeletalMeshComponent"));
	ESkeletalMeshComponent->SetupAttachment(ECapsuleComponent);
	ESkeletalMeshComponent->SetRelativeLocation(FVector(0, 0, 0));

	ESpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("ESpringArm"));
	ESpringArmComponent->TargetArmLength = 400.0f;
	ESpringArmComponent->SetupAttachment(RootComponent);

	ECameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ECamera"));
	ECameraComponent->SetupAttachment(ESpringArmComponent);

	ESkeletalMesh = LoadObject<USkeletalMesh>(NULL, TEXT("SkeletalMesh'/Game/ParagonWraith/Characters/Heroes/Wraith/Meshes/Wraith.Wraith'"));

	EPoseableMeshComponent = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("EPoseableMeshComponent"));
	EPoseableMeshComponent->SetupAttachment(ECapsuleComponent);
	EPoseableMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
	EPoseableMeshComponent->SetSkeletalMesh(ESkeletalMesh);

}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	//ik = (UCCD_IK*)(Arm->GetComponentByClass(UCCD_IK::StaticClass()));
	//ikDefaultPosition = actorMovingAlongSpline->GetActorLocation();
	Super::BeginPlay();
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::CCD_IK_Init() {
	TArray<UPoseableMeshComponent*> PoseableMeshComponents;
	TArray<FBoneInfo> TempBonesInfo;
	this->GetComponents(PoseableMeshComponents);

	Component = PoseableMeshComponents[0];

	FBoneSensor BoneSensor(Component);
	TempBonesInfo = BoneSensor.GetPoseableBonesInfo();

	FBoneInfo init_use;
	bones.Init(init_use, 6);
	for (auto bone : TempBonesInfo)
	{
		FString n = bone.BoneName;
		if (n.Compare("spine_02") == 0)
			bones[0] = bone;
		else if (n.Compare("spine_03") == 0)
			bones[1] = bone;
		else if (n.Compare("clavicle_l") == 0)
			bones[2] = bone;
		else if (n.Compare("upperarm_l") == 0)
			bones[3] = bone;
		else if (n.Compare("lowerarm_l") == 0)
			bones[4] = bone;
		else if (n.Compare("hand_l") == 0)
			bones[5] = bone;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s"), *bones[5].BoneName));

	for (int i = 0; i < bones.Num(); i++)
	{
		//finding out which local axis is directed towards child's position
		if (i < bones.Num() - 1)
		{
			bonesLength.Add((bones[i].ComponentTM.GetLocation() - bones[i + 1].ComponentTM.GetLocation()).Size());
			chainLength += bonesLength[i];
		}
		BonesPosition.Add(bones[i].BoneTM.GetLocation());
		BonesRotation.Add(bones[i].BoneTM.GetRotation());
	}

}

void ANPC::CCD_IK_Solve(FVector targetPosition) {
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *(targetPosition.ToString()));
	for (int i = bones.Num() - 2; i > -1; i--)
	{
		//CCD tends to overemphasise the rotations of the bones closer to the target position.
		//we can reduce this by reducing weight down the hierarchy but for now lets just complete this without weights
		FVector toLastBone = bones[bones.Num() - 1].ComponentTM.GetLocation() - bones[i].ComponentTM.GetLocation();

		FVector toTarget = targetPosition - bones[i].ComponentTM.GetLocation();

		//Get the rotation to direct the last bone towards target
		
		FQuat targetRotation = FromToRotation(toLastBone, toTarget) * bones[i].ComponentTM.GetRotation();

		//Component->SetBoneRotationByName(bones[i], targetRot, EBoneSpaces::WorldSpace);
		Component->SetWorldRotation(targetRotation);
	}
}

void ANPC::CCD_IK_Update() {

	for (int i = 0; i < bones.Num(); i++)
	{
		FVector NowBonePosition = bones[i].BoneTM.GetLocation();
		FQuat NowBoneRotation = bones[i].BoneTM.GetRotation();

		if (NowBonePosition != BonesPosition[i] || NowBoneRotation != BonesRotation[i]) {
			is_effector_transform = true;
		}
	}

}

void ANPC::CCD_IK_FixTransform()
{
	for (int i = 0; i < bones.Num(); i++)
	{
		if (bones[i].BoneTM.GetLocation() != BonesPosition[i]) Component->SetRelativeLocation(BonesPosition[i]);
		if (bones[i].BoneTM.GetRotation() != BonesRotation[i]) Component->SetRelativeRotation(BonesRotation[i]);
	}
}


//-------------------------tool-----------------------------
FQuat ANPC::AngleAxis(float angle, FVector axis)
{
	axis.Normalize();
	float rad = FMath::DegreesToRadians(angle) * 0.5f;
	axis *= FMath::Sin(rad);
	return FQuat(axis.X, axis.Y, axis.Z, FMath::Cos(rad));
}

FQuat ANPC::FromToRotation(FVector from, FVector to)
{
	from.Normalize();
	to.Normalize();
	FVector axis = FVector::CrossProduct(from, to);
	float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(from.GetSafeNormal(), to.GetSafeNormal())));
	return AngleAxis(angle, axis);
}
