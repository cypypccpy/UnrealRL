#include "FBoneInfo.h"
// #include "Runtime/Engine/Public/BoneIndices.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h" 
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Public/AnimationRuntime.h"

FBoneSensor::FBoneSensor(const UPoseableMeshComponent* InPoseableMeshComponent) {
	this->PoseableComponent = InPoseableMeshComponent;
}


void FBoneSensor::SetBones(const TArray<FString>& InIncludedBoneNames)
{
	IncludedBoneNames = InIncludedBoneNames;
}

TArray<FBoneInfo> FBoneSensor::GetPoseableBonesInfo()
{
	TArray<FBoneInfo> BonesInfo;

	const FBoneContainer& RequiredBones = PoseableComponent->RequiredBones;
	USkeletalMesh* SkeletalMesh = PoseableComponent->SkeletalMesh;
	const FTransformArrayA2& ComponentSpaceTransforms = PoseableComponent->GetComponentSpaceTransforms();
	const FTransformArrayA2& BoneSpaceTransforms = PoseableComponent->BoneSpaceTransforms;
	const FTransform& ComponentToWorld = PoseableComponent->GetComponentToWorld();

	bool bIncludeAll = false;
	if (IncludedBoneNames.Num() == 0 || (IncludedBoneNames.Num() == 1 && IncludedBoneNames[0].IsEmpty()))
	{
		bIncludeAll = true;
	}

	for (int32 Index = 0; Index < RequiredBones.GetBoneIndicesArray().Num(); ++Index)
	{
		int32 BoneIndex = RequiredBones.GetBoneIndicesArray()[Index];
		FName BoneName = SkeletalMesh->RefSkeleton.GetBoneName(BoneIndex);
		if (!bIncludeAll
			&& !IncludedBoneNames.Contains(BoneName.ToString()))
		{
			continue;
		}

		// Skip if the bone is not what we need

		FTransform BoneTM = BoneSpaceTransforms[BoneIndex];
		FTransform ComponentTM = ComponentSpaceTransforms[BoneIndex];
		FTransform WorldTM = ComponentTM * ComponentToWorld;

		/*
		int32 ParentIndex = SkeletalMesh->RefSkeleton.GetParentIndex(BoneIndex);

		FVector Start, End;

		End = BoneTM.GetLocation();
		if (ParentIndex >= 0)
		{
			Start = (ComponentSpaceTransforms[ParentIndex] * ComponentToWorld).GetLocation();
		}
		else // No parent??
		{
			Start = ComponentToWorld.GetLocation();
		}
		*/

		FBoneInfo BoneInfo;
		BoneInfo.BoneName = BoneName.ToString();
		BoneInfo.BoneTM = BoneTM;
		BoneInfo.ComponentTM = ComponentTM;
		BoneInfo.WorldTM = WorldTM;

		BonesInfo.Add(BoneInfo);
		// UE_LOG(LogTemp, Log, TEXT("Bone: %d, Start: %s, End: %s"), BoneIndex, *Start.ToString(), *End.ToString());
	}
	return BonesInfo;
}
