// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h" 
#include "Components/PoseableMeshComponent.h" 
#include "FBoneInfo.h"
#include "Camera/CameraComponent.h"
#include "NPC.generated.h"


UCLASS()

class MANIPULATOR_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void CCD_IK_Init();

	void CCD_IK_Solve(FVector targetPosition);

	void CCD_IK_Update();

	void CCD_IK_FixTransform();

	//------------tool-------------
	FQuat FromToRotation(FVector from, FVector to);

	FQuat AngleAxis(float angle, FVector axis);


public:
	UPoseableMeshComponent* Component;

	TArray<FVector, TFixedAllocator<6>> BonesPosition;
	TArray<FQuat, TFixedAllocator<6>> BonesRotation;

	TArray<float, TFixedAllocator<6>> bonesLength;

	TArray<FBoneInfo, TFixedAllocator<6>> bones;

	int32 chainLength;

	bool is_effector_transform = false;

public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		UCapsuleComponent* ECapsuleComponent;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		USpringArmComponent* ESpringArmComponent;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		USkeletalMeshComponent* ESkeletalMeshComponent;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		UCameraComponent* ECameraComponent;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		USkeletalMesh* ESkeletalMesh;
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "LV")
		UPoseableMeshComponent* EPoseableMeshComponent;

private:
	FVector MoveInput;
};
