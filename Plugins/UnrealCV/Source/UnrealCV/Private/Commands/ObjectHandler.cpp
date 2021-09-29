// Weichao Qiu @ 2017
#include "DelayAction.h"
#include "ObjectHandler.h"

FExecStatus SetActorName(AActor* Actor, FString NewName)
{
	UObject* NameScopeOuter = ANY_PACKAGE;
	UObject* ExistingObject = StaticFindObject(/*Class=*/ NULL, NameScopeOuter, *NewName, true);
	if (IsValid(ExistingObject))
	{
		if (ExistingObject == Actor)
		{
			FString ErrorMsg = TEXT("The name has already been set");
			return FExecStatus::OK(ErrorMsg);
		}
		else if (ExistingObject)
		{
			FString ErrorMsg = FString::Printf(TEXT("Renaming an object to overwrite an existing object is not allowed, %s"),  *NewName);
			return FExecStatus::Error(ErrorMsg);
		}
	}
	else
	{
		ICommandInterface* CmdActor = Cast<ICommandInterface>(Actor);
		if (CmdActor) CmdActor->UnbindCommands();
		Actor->Rename(*NewName);
		if (CmdActor) CmdActor->BindCommands();
	}
	return FExecStatus::OK();
}

void FObjectHandler::RegisterCommands()
{
	CommandDispatcher->BindCommand(
		"vset /object/[str]/[str]/position [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetEffectorPosition),
		"Get the object's skeletalmesh"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/bones",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetSkeletalMesh),
		"Get the object's skeletalmesh"
	);

	CommandDispatcher->BindCommand(
		"vget /objects",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetObjectList),
		"Get the name of all objects"
	);

	CommandDispatcher->BindCommand(
		"vset /objects/spawn_cube",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SpawnBox),
		"Spawn a box in the scene for debugging purpose."
	);

	CommandDispatcher->BindCommand(
		"vset /objects/spawn_cube [str]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SpawnBox),
		"Spawn a box in the scene for debugging purpose, with optional argument name."
	);

	CommandDispatcher->BindCommand(
		"vset /objects/spawn [str]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::Spawn),
		"Spawn an object with UClassName as the argument."
	);

	CommandDispatcher->BindCommand(
		"vset /objects/spawn [str] [str]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::Spawn),
		"Spawn an object with UClassName as the argument."
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/location",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetLocation),
		"Get object location [x, y, z]"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/location [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetLocation),
		"Set object location [x, y, z]"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/rotation",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetRotation),
		"Get object rotation [pitch, yaw, roll]"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/rotation [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetRotation),
		"Set object rotation [pitch, yaw, roll]"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/vertex_location",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetObjectVertexLocation),
		"Get vertex location"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/color",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetAnnotationColor),
		"Get the labeling color of an object (used in object instance mask)"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/color [uint] [uint] [uint]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetAnnotationColor),
		"Set the labeling color of an object [r, g, b]"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/mobility",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetMobility),
		"Is the object static or movable?"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/show",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetShow),
		"Show object"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/hide",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetHide),
		"Hide object"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/destroy",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::Destroy),
		"Destroy object"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/uclass_name",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetUClassName),
		"Get the UClass name for filtering objects"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/name [str]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetName),
		"Set the name of the object"
	);

#if WITH_EDITOR
	CommandDispatcher->BindCommand(
		"vget /object/[str]/label",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetActorLabel),
		"Get actor label"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/label [str]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetActorLabel),
		"Set actor label"
	);
#endif

	CommandDispatcher->BindCommand(
		"vget /object/[str]/scale",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetScale),
		"Get object rotation [x, y, z]"
	);

	CommandDispatcher->BindCommand(
		"vset /object/[str]/scale [float] [float] [float]",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::SetScale),
		"Set object scale [x, y, z]"
	);

	CommandDispatcher->BindCommand(
		"vget /object/[str]/bounds",
		FDispatcherDelegate::CreateRaw(this, &FObjectHandler::GetBounds),
		"Return the bounds in the world coordinate, formate is [minx, y, z, maxx, y, z]"
	);
}

AActor* GetActor(const TArray<FString>& Args)
{
	FString ActorId = Args[0];
	AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
	return Actor;
}

FExecStatus FObjectHandler::SetEffectorPosition(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	BoneActor = Actor;
	TArray<UPoseableMeshComponent*> PoseableMeshComponents;
	Actor->GetComponents(PoseableMeshComponents);
	Component = PoseableMeshComponents[0];

	CCD_IK_Init(Component);

	FString log;

	// TODO: Check whether this object is movable
	float X = FCString::Atof(*Args[2]), Y = FCString::Atof(*Args[3]), Z = FCString::Atof(*Args[4]);
	targetLocation = bones[5].WorldTM.GetLocation() + FVector(X, Y, Z);

	FVector singularityOffset = CCD_IK_GetSingularityOffset();

	//FixTransform();
	//iterating the solver
	for (int i = 0; i < maxIterations; i++)
	{
		//optimizations
		//if (singularityOffset == FVector::ZeroVector && i >= 1 && tolerance > 0) break;
		CCD_IK_Solve(targetLocation);
	}

	for (auto BoneInfo : bones) {
		log += BoneInfo.ToString() + "\n";
	}
	return FExecStatus::OK(log);
}

FExecStatus FObjectHandler::GetSkeletalMesh(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	TArray<UPoseableMeshComponent*> PoseableMeshComponents;
	Actor->GetComponents(PoseableMeshComponents);

	// FString StrActorGetSkeletalMesh;
	FString StrBoneInfo;
	for (int32 b = 0; b < PoseableMeshComponents.Num(); b++)
	{
		UPoseableMeshComponent* g_Component = PoseableMeshComponents[b];

		FBoneSensor BoneSensor(g_Component);
		TArray<FBoneInfo> BonesInfo = BoneSensor.GetPoseableBonesInfo();
		
		for (auto BoneInfo : BonesInfo) {
			StrBoneInfo += BoneInfo.ToString() + "\n";
		}
		StrBoneInfo += FString::Printf(TEXT("----------------------------------\n"));
		// StrActorGetSkeletalMesh += FString::Printf(TEXT("%s\n"), *Component->GetName());
	}

	return FExecStatus::OK(StrBoneInfo);
}

FExecStatus FObjectHandler::GetObjectList(const TArray<FString>& Args)
{
	TArray<AActor*> ActorList;
	UVisionBPLib::GetActorList(ActorList);

	FString StrActorList;
	for (AActor* Actor : ActorList)
	{
		StrActorList += FString::Printf(TEXT("%s\n"), *Actor->GetName());
	}
	return FExecStatus::OK(StrActorList);
}

FExecStatus FObjectHandler::GetLocation(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FActorController Controller(Actor);
	FVector Location = Controller.GetLocation();

	FStrFormatter Ar;
	Ar << Location;

	return FExecStatus::OK(Ar.ToString());
}

/** There is no guarantee this will always succeed, for example, hitting a wall */
FExecStatus FObjectHandler::SetLocation(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");
	FActorController Controller(Actor);

	// TODO: Check whether this object is movable
	float X = FCString::Atof(*Args[1]), Y = FCString::Atof(*Args[2]), Z = FCString::Atof(*Args[3]);
	FVector NewLocation = FVector(X, Y, Z);
	Controller.SetLocation(NewLocation);

	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetRotation(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FActorController Controller(Actor);
	FRotator Rotation = Controller.GetRotation();

	FStrFormatter Ar;
	Ar << Rotation;

	return FExecStatus::OK(Ar.ToString());
}

FExecStatus FObjectHandler::SetRotation(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");
	FActorController Controller(Actor);

	// TODO: Check whether this object is movable
	float Pitch = FCString::Atof(*Args[1]), Yaw = FCString::Atof(*Args[2]), Roll = FCString::Atof(*Args[3]);
	FRotator Rotator = FRotator(Pitch, Yaw, Roll);
	Controller.SetRotation(Rotator);

	return FExecStatus::OK();
}


FExecStatus FObjectHandler::GetAnnotationColor(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");
	FActorController Controller(Actor);

	FColor AnnotationColor;
	Controller.GetAnnotationColor(AnnotationColor);
	return FExecStatus::OK(AnnotationColor.ToString());
}

FExecStatus FObjectHandler::SetAnnotationColor(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");
	FActorController Controller(Actor);

	// ObjectName, R, G, B, A = 255
	// The color format is RGBA
	uint32 R = FCString::Atoi(*Args[1]), G = FCString::Atoi(*Args[2]), B = FCString::Atoi(*Args[3]), A = 255; // A = FCString::Atoi(*Args[4]);
	FColor AnnotationColor(R, G, B, A);

	Controller.SetAnnotationColor(AnnotationColor);
	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetMobility(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");
	FActorController Controller(Actor);

	FString MobilityName = "";
	EComponentMobility::Type Mobility = Controller.GetMobility();
	switch (Mobility)
	{
		case EComponentMobility::Type::Movable: MobilityName = "Movable"; break;
		case EComponentMobility::Type::Static: MobilityName = "Static"; break;
		case EComponentMobility::Type::Stationary: MobilityName = "Stationary"; break;
		default: MobilityName = "Unknown";
	}
	return FExecStatus::OK(MobilityName);
}

FExecStatus FObjectHandler::SetShow(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FActorController Controller(Actor);
	Controller.Show();
	return FExecStatus::OK();
}

FExecStatus FObjectHandler::SetHide(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FActorController Controller(Actor);
	Controller.Hide();
	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetObjectVertexLocation(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	FVertexSensor VertexSensor(Actor);
	TArray<FVector> VertexArray = VertexSensor.GetVertexArray();

	// Serialize it to json?
	FString Str = "";
	for (auto Vertex : VertexArray)
	{
		FString VertexLocation = FString::Printf(
			TEXT("%.5f     %.5f     %.5f"),
			Vertex.X, Vertex.Y, Vertex.Z);
		Str += VertexLocation + "\n";
	}

	return FExecStatus::OK(Str);
}

/** A debug utility function to create StaticBox through python API */
FExecStatus FObjectHandler::SpawnBox(const TArray<FString>& Args)
{
	FString ObjectName;
	if (Args.Num() == 1) { ObjectName = Args[0]; }

	UWorld* GameWorld = FUnrealcvServer::Get().GetWorld();
	AActor* Actor = GameWorld->SpawnActor(ACubeActor::StaticClass());

	return FExecStatus::OK();
}

FExecStatus FObjectHandler::Spawn(const TArray<FString>& Args)
{
	if (Args.Num() == 2)
	{
		FString ActorId = Args[1];
		AActor* Actor = GetActorById(FUnrealcvServer::Get().GetWorld(), ActorId);
		if (IsValid(Actor))
		{
			FString ErrorMsg = FString::Printf(TEXT("Failed to spawn %s, object exsit."), *ActorId);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
			return FExecStatus::Error(ErrorMsg);
		}
	}

	FString UClassName;
	if (Args.Num() == 1 || Args.Num() == 2)
	{ 
		UClassName = Args[0]; 
	}
	// Lookup UClass with a string
	UClass*	Class = FindObject<UClass>(ANY_PACKAGE, *UClassName);

	if (!IsValid(Class))
	{
		FString ErrorMsg = FString::Printf(TEXT("Can not find a class with name '%s'"), *UClassName);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ErrorMsg);
		return FExecStatus::Error(ErrorMsg);
	}

	UWorld* GameWorld = FUnrealcvServer::Get().GetWorld();
	FActorSpawnParameters SpawnParameters;
	// SpawnParameters.bNoFail = true; // Allow collision during spawn.
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Actor = GameWorld->SpawnActor(Class, NULL, NULL, SpawnParameters);
	if (!IsValid(Actor))
	{
		return FExecStatus::Error("Failed to spawn actor");
	}

	if (Args.Num() == 2)
	{
		FString ActorName = Args[1];
		SetActorName(Actor, ActorName);
	}
	return FExecStatus::OK(Actor->GetName());
}

FExecStatus FObjectHandler::Destroy(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	Actor->Destroy();
	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetUClassName(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FString UClassName = Actor->StaticClass()->GetName();
	return FExecStatus::OK(UClassName);
}

/** Get component names of the object */
FExecStatus FObjectHandler::GetComponents(const TArray<FString>& Args)
{
	return FExecStatus::OK();
}


FExecStatus FObjectHandler::SetName(const TArray<FString>& Args)
{
	if (Args.Num() != 2)
	{
		return FExecStatus::InvalidArgument;
	}
	AActor* Actor = GetActor(Args);
	if (!Actor) return FExecStatus::Error("Can not find object");

	FString NewName = Args[1];
	// Check whether the object name already exists, otherwise it will crash in 
	// File:/home/qiuwch/UnrealEngine/Engine/Source/Runtime/CoreUObject/Private/UObject/Obj.cpp] [Line: 198]
	FExecStatus Status = SetActorName(Actor, NewName);

	return Status; 
}

#if WITH_EDITOR
FExecStatus FObjectHandler::SetActorLabel(const TArray<FString>& Args)
{
	FString ActorLabel;
	if (Args.Num() == 2) { ActorLabel = Args[1]; }

	AActor* Actor = GetActor(Args);
	if (!IsValid(Actor)) return FExecStatus::Error("Can not find object");

	Actor->SetActorLabel(ActorLabel);
	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetActorLabel(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!IsValid(Actor)) return FExecStatus::Error("Can not find object");

	FString ActorLabel = Actor->GetActorLabel();
	return FExecStatus::OK(ActorLabel);
}
#endif 


FExecStatus FObjectHandler::GetScale(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!IsValid(Actor)) return FExecStatus::Error("Can not find object");

	FVector Scale = Actor->GetActorScale3D();

	FStrFormatter Ar;
	Ar << Scale;

	return FExecStatus::OK(Ar.ToString());
}

FExecStatus FObjectHandler::SetScale(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!IsValid(Actor)) return FExecStatus::Error("Can not find object");

	float X = FCString::Atof(*Args[1]), Y = FCString::Atof(*Args[2]), Z = FCString::Atof(*Args[3]);
	FVector Scale = FVector(X, Y, Z);
	Actor->SetActorScale3D(Scale);

	return FExecStatus::OK();
}

FExecStatus FObjectHandler::GetBounds(const TArray<FString>& Args)
{
	AActor* Actor = GetActor(Args);
	if (!IsValid(Actor)) return FExecStatus::Error("Can not find object");

	bool bOnlyCollidingComponents = false;
	FVector Origin, BoundsExtent;
	Actor->GetActorBounds(bOnlyCollidingComponents, Origin, BoundsExtent);  
	FVector Min = Origin - BoundsExtent, Max = Origin + BoundsExtent;

	FString Res = FString::Printf(TEXT("%.2f %.2f %.2f %.2f %.2f %.2f"), 
		Min.X, Min.Y, Min.Z, Max.X, Max.Y, Max.Z);

	return FExecStatus::OK(Res);
}

void FObjectHandler::CCD_IK_Init(UPoseableMeshComponent* Component_) {

	Component = Component_;

	FBoneSensor BoneSensor(Component);
	TArray<FBoneInfo> TempBonesInfo;

	TempBonesInfo = BoneSensor.GetPoseableBonesInfo();

	if (!is_inited) {
		FVector v;
		FQuat q;
		FBoneInfo init_use;
		bones.Init(init_use, 6);
		bonesLength.Init(0.0, 6);
		BonesPosition.Init(v, 6);
		BonesRotation.Init(q, 6);
		is_inited = true;
	}

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

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Effector: %s"), *bones[5].BoneName));

	for (int i = 0; i < bones.Num(); i++)
	{
		//finding out which local axis is directed towards child's position
		if (i < bones.Num() - 1)
		{
			bonesLength[i] = (bones[i].ComponentTM.GetLocation() - bones[i + 1].ComponentTM.GetLocation()).Size();
			chainLength += bonesLength[i];
		}
		BonesPosition[i] = bones[i].BoneTM.GetLocation();
		BonesRotation[i] = bones[i].BoneTM.GetRotation();
	}

}

void FObjectHandler::CCD_IK_Solve(FVector targetPosition) {
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *(targetPosition.ToString()));
	for (int i = bones.Num() - 2; i > -1; i--)
	{
		//CCD tends to overemphasise the rotations of the bones closer to the target position.
		//we can reduce this by reducing weight down the hierarchy but for now lets just complete this without weights
		FVector toLastBone = bones[bones.Num() - 1].ComponentTM.GetLocation() - bones[i].ComponentTM.GetLocation();

		FVector toTarget = targetPosition - bones[i].ComponentTM.GetLocation();

		//Get the rotation to direct the last bone towards target

		FQuat targetRotation = FromToRotation(toLastBone, toTarget) * bones[i].ComponentTM.GetRotation();

		//Component->SetBoneRotationByName(FName(bones[i].BoneName), FRotator(targetRotation), EBoneSpaces::WorldSpace);
		CCD_IK_InterpolationMove(i, targetRotation);
	}
}

void FObjectHandler::CCD_IK_Update(FVector NewLocation) {

	

}

void FObjectHandler::CCD_IK_FixTransform()
{
	for (int i = 0; i < bones.Num(); i++)
	{
		//if (bones[i].BoneTM.GetLocation() != BonesPosition[i]) Component->SetBoneRotationByName(BonesPosition[i]);
		//if (bones[i].BoneTM.GetRotation() != BonesRotation[i]) Component->SetBoneRotationByName(BonesRotation[i]);
	}
}

//Get target offset to break out of linear singularity issue
FVector FObjectHandler::CCD_IK_GetSingularityOffset()
{
	if (!CCD_IK_SingularityDetected()) return FVector::ZeroVector;

	FVector IKDirection = (targetLocation - bones[0].ComponentTM.GetLocation()).GetSafeNormal();
	FVector secondaryDirection = bones[bones.Num() - 2].ComponentTM.GetRotation() * FVector(1, 0, 0);

	return FVector::CrossProduct(IKDirection, secondaryDirection) * bonesLength[bonesLength.Num() - 2] * 0.5f;
}

//Detects linear singularity if vector from FIRST bone to TARGET matches the vector from FIRST bone to LAST bone. 
bool FObjectHandler::CCD_IK_SingularityDetected()
{
	FVector toLastBone = bones[bones.Num() - 1].ComponentTM.GetLocation() - bones[0].ComponentTM.GetLocation();
	FVector toTarget = targetLocation - bones[0].ComponentTM.GetLocation();

	float toLastBoneDistance = toLastBone.Size();
	float toTargetDistance = toTarget.Size();

	if (toLastBoneDistance == 0 || toTargetDistance == 0 || toLastBoneDistance < toTargetDistance) return false;
	if (toLastBoneDistance < chainLength - (bonesLength[bonesLength.Num() - 2] * 0.1f)) return false;

	float dot = FVector::DotProduct(toLastBone.GetUnsafeNormal(), toTarget.GetUnsafeNormal());
	if (dot < 0.99f) return false;

	return true;
}

void FObjectHandler::CCD_IK_InterpolationMove(int bone_num, FQuat targetRotation_) {
	FQuat NowBoneRotation = bones[bone_num].WorldTM.GetRotation();
	for (int j = 0; j < 10; j++) {
		FRotator MoveBoneRotation = FRotator(targetRotation_) + (FRotator(targetRotation_) - FRotator(NowBoneRotation)) * (j / 10);
		//UWorld* world = this->GetWorld();
		//if (world)
		//{
		//	world->GetTimerManager().SetTimer(MemberTimerHandle, this, &FObjectHandler::RepeatingFunction, 0.016f, false, 0.0f);
		//}
		Component->SetBoneRotationByName(FName(bones[bone_num].BoneName), MoveBoneRotation, EBoneSpaces::WorldSpace);
		CustomDelay(BoneActor, 0.5f);
	}
}

void FObjectHandler::RepeatingFunction() {
}

void FObjectHandler::CustomDelay(UObject* WorldContextObject, float Duration) {
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = WorldContextObject;
		LatentInfo.ExecutionFunction = TEXT("OnDelayFinished");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = FMath::Rand();
		LatentActionManager.AddNewAction(WorldContextObject, LatentInfo.UUID, new FDelayAction(Duration, LatentInfo));
	}
}

//-------------------------tool-----------------------------
FQuat FObjectHandler::AngleAxis(float angle, FVector axis)
{
	axis.Normalize();
	float rad = FMath::DegreesToRadians(angle) * 0.5f;
	axis *= FMath::Sin(rad);
	return FQuat(axis.X, axis.Y, axis.Z, FMath::Cos(rad));
}

FQuat FObjectHandler::FromToRotation(FVector from, FVector to)
{
	from.Normalize();
	to.Normalize();
	FVector axis = FVector::CrossProduct(from, to);
	float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(from.GetSafeNormal(), to.GetSafeNormal())));
	return AngleAxis(angle, axis);
}