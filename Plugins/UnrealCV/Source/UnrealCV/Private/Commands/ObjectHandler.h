// Weichao Qiu @ 2016
#pragma once
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Runtime/CoreUObject/Public/UObject/Package.h"

#include "UnrealcvServer.h"
#include "Controller/ActorController.h"
#include "VertexSensor.h"
#include "Utils/StrFormatter.h"
#include "Utils/UObjectUtils.h"
#include "VisionBPLib.h"
#include "CubeActor.h"
#include "CommandInterface.h"
#include "BoneSensor.h"
#include "TimerManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Components/PoseableMeshComponent.h" 
#include "CommandDispatcher.h"
#include "CommandHandler.h"

/** 
 * Handle vget/vset /object/ commands.
 * The definition of object is any actor contains a MeshComponent in the runtime
 * This includes StaticMesh, SkeletalMesh, ProceduralMesh (tree), Landscape, Foliage
 * Object is different than actor, only actor contains mesh can be called as object
*/
class FObjectHandler : public FCommandHandler
{
public:
	void RegisterCommands();

private:
	// See RegisterCommands in ObjectHandler.cpp for what each function is doing.
	FExecStatus SetEffectorPosition(const TArray<FString>& Args);
	
	FExecStatus GetSkeletalMesh(const TArray<FString>& Args);

	FExecStatus GetObjectList(const TArray<FString>& Args);

	FExecStatus SpawnBox(const TArray<FString>& Args);

	FExecStatus Spawn(const TArray<FString>& Args);

	FExecStatus GetAnnotationColor(const TArray<FString>& Args);

	FExecStatus SetAnnotationColor(const TArray<FString>& Args);

	FExecStatus GetObjectName(const TArray<FString>& Args);

	FExecStatus CurrentObjectHandler(const TArray<FString>& Args);

	FExecStatus GetLocation(const TArray<FString>& Args);

	FExecStatus GetRotation(const TArray<FString>& Args);

	FExecStatus SetLocation(const TArray<FString>& Args);

	FExecStatus SetRotation(const TArray<FString>& Args);

	FExecStatus SetShow(const TArray<FString>& Args);

	FExecStatus SetHide(const TArray<FString>& Args);

	FExecStatus GetMobility(const TArray<FString>& Args);

	FExecStatus GetObjectVertexLocation(const TArray<FString>& Args);

	FExecStatus Destroy(const TArray<FString>& Args);

	FExecStatus GetUClassName(const TArray<FString>& Args);

	FExecStatus GetComponents(const TArray<FString>& Args);

	FExecStatus SetName(const TArray<FString>& Args);

#if WITH_EDITOR
	FExecStatus SetActorLabel(const TArray<FString>& Args);

	FExecStatus GetActorLabel(const TArray<FString>& Args);
#endif

	FExecStatus GetScale(const TArray<FString>& Args);

	FExecStatus SetScale(const TArray<FString>& Args);

	FExecStatus GetBounds(const TArray<FString>& Args);

public:
	void CCD_IK_Init(UPoseableMeshComponent* Component_);

	void CCD_IK_Solve(FVector targetPosition);

	void CCD_IK_Update(FVector NewLocation_);

	void CCD_IK_FixTransform();

	FVector CCD_IK_GetSingularityOffset();

	bool CCD_IK_SingularityDetected();

	void CCD_IK_InterpolationMove(int bone_num, FQuat targetRotation_);

	//------------tool-------------
	FQuat FromToRotation(FVector from, FVector to);

	FQuat AngleAxis(float angle, FVector axis);

	UPoseableMeshComponent* Component;

	TArray<FVector, TFixedAllocator<6>> BonesPosition;
	TArray<FQuat, TFixedAllocator<6>> BonesRotation;

	TArray<float, TFixedAllocator<6>> bonesLength;

	TArray<FBoneInfo, TFixedAllocator<6>> bones;

	FVector targetLocation;

	int32 chainLength;

	int32 maxIterations = 25;

	bool is_effector_transform = false;

	bool is_inited = false;

	void RepeatingFunction();

	FTimerHandle MemberTimerHandle;

	static void CustomDelay(UObject* WorldContextObject, float Duration = 2.0f);

	AActor* BoneActor;
};
