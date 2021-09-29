// Weichao Qiu @ 2017
#pragma once

#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h" 
#include "CoreMinimal.h"

/**
 * Information to describe a bone in a SkeletalMesh
 */
struct FBoneInfo
{
	FString BoneName;
	FTransform BoneTM;
	FTransform ComponentTM;
	FTransform WorldTM;

	FString ToString()
	{
		return FString::Printf(TEXT("Bone: %s, %s"), *BoneName, *ComponentTM.ToHumanReadableString());
	}
};

/** 
 * A native BoneSensor, use VisionBPLib to involve in blueprint 
 */
class UNREALCV_API FBoneSensor
{
public:
	/** Construct a BoneSensor to extract data from a SkeletalMeshComponent */
	FBoneSensor(const class USkeletalMeshComponent* InSkeletalMeshComponent);
	FBoneSensor(const class UPoseableMeshComponent* InPoseableMeshComponent);

	/** The bone names this BoneSensor should extract */
	void SetBones(const TArray<FString>& InIncludedBoneNames);

	/** Read information and return as an array */
	TArray<FBoneInfo> GetBonesInfo();
	TArray<FBoneInfo> GetPoseableBonesInfo();

	/** Only track the bone information specified in this list */
	TArray<FString> IncludedBoneNames;

private:
	const USkeletalMeshComponent* Component;
	const UPoseableMeshComponent* PoseableComponent;
};
