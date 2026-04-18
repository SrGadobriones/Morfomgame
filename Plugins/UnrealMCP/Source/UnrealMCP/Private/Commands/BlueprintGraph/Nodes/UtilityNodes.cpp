#include "Commands/BlueprintGraph/Nodes/UtilityNodes.h"
#include "Commands/BlueprintGraph/Nodes/NodeCreatorUtils.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Select.h"
#include "K2Node_SpawnActorFromClass.h"
#include "EdGraphSchema_K2.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Json.h"

UK2Node* FUtilityNodeCreator::CreatePrintNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	UK2Node_CallFunction* PrintNode = NewObject<UK2Node_CallFunction>(Graph);
	if (!PrintNode)
	{
		return nullptr;
	}

	UFunction* PrintFunc = UKismetSystemLibrary::StaticClass()->FindFunctionByName(
		GET_FUNCTION_NAME_CHECKED(UKismetSystemLibrary, PrintString)
	);

	if (!PrintFunc)
	{
		return nullptr;
	}

	// Set function reference BEFORE initialization
	PrintNode->SetFromFunction(PrintFunc);

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	PrintNode->NodePosX = static_cast<int32>(PosX);
	PrintNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(PrintNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(PrintNode, Graph);

	// Set message if provided AFTER initialization
	FString Message;
	if (Params->TryGetStringField(TEXT("message"), Message))
	{
		UEdGraphPin* InStringPin = PrintNode->FindPin(TEXT("InString"));
		if (InStringPin)
		{
			InStringPin->DefaultValue = Message;
		}
	}

	return PrintNode;
}

UK2Node* FUtilityNodeCreator::CreateCallFunctionNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	// Get target function name
	FString TargetFunction;
	if (!Params->TryGetStringField(TEXT("target_function"), TargetFunction))
	{
		return nullptr;
	}

	UK2Node_CallFunction* CallNode = NewObject<UK2Node_CallFunction>(Graph);
	if (!CallNode)
	{
		return nullptr;
	}

	// Find the function to call
	UFunction* TargetFunc = nullptr;
	FString ClassName;
	bool bHasTargetClass = Params->TryGetStringField(TEXT("target_class"), ClassName);

	auto FindFuncInClass = [&TargetFunction](UClass* Cls) -> UFunction* {
		if (!Cls) return nullptr;
		return Cls->FindFunctionByName(FName(*TargetFunction));
	};

	auto LoadClassFlexible = [](const FString& Name) -> UClass* {
		if (Name.IsEmpty()) return nullptr;
		if (Name.StartsWith(TEXT("/")))
		{
			if (UClass* C = LoadObject<UClass>(nullptr, *Name)) return C;
		}
		if (UClass* C = UClass::TryFindTypeSlow<UClass>(Name)) return C;
		static const TCHAR* Prefixes[] = {
			TEXT("/Script/Engine."),
			TEXT("/Script/UMG."),
			TEXT("/Script/CoreUObject."),
			TEXT("/Script/EnhancedInput."),
			TEXT("/Script/GameplayAbilities."),
			TEXT("/Script/AIModule."),
		};
		for (const TCHAR* P : Prefixes)
		{
			if (UClass* C = LoadObject<UClass>(nullptr, *(FString(P) + Name))) return C;
		}
		return nullptr;
	};

	if (bHasTargetClass && !ClassName.IsEmpty())
	{
		UClass* TargetClass = LoadClassFlexible(ClassName);
		TargetFunc = FindFuncInClass(TargetClass);
	}
	else
	{
		// Auto-search common Unreal libraries
		static const TCHAR* CandidateClasses[] = {
			TEXT("/Script/Engine.KismetMathLibrary"),
			TEXT("/Script/Engine.KismetSystemLibrary"),
			TEXT("/Script/Engine.GameplayStatics"),
			TEXT("/Script/Engine.KismetStringLibrary"),
			TEXT("/Script/Engine.KismetArrayLibrary"),
			TEXT("/Script/Engine.KismetTextLibrary"),
			TEXT("/Script/Engine.KismetInputLibrary"),
		};
		for (const TCHAR* Path : CandidateClasses)
		{
			if (UClass* C = LoadObject<UClass>(nullptr, Path))
			{
				TargetFunc = FindFuncInClass(C);
				if (TargetFunc) break;
			}
		}
	}

	if (!TargetFunc)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateCallFunctionNode: Function '%s' not found in class '%s'"),
			*TargetFunction, bHasTargetClass ? *ClassName : TEXT("<auto>"));
		return nullptr;
	}

	// Set function reference BEFORE initialization
	CallNode->SetFromFunction(TargetFunc);

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	CallNode->NodePosX = static_cast<int32>(PosX);
	CallNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(CallNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(CallNode, Graph);

	return CallNode;
}

UK2Node* FUtilityNodeCreator::CreateSelectNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	UK2Node_Select* SelectNode = NewObject<UK2Node_Select>(Graph);
	if (!SelectNode)
	{
		return nullptr;
	}

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	SelectNode->NodePosX = static_cast<int32>(PosX);
	SelectNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(SelectNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(SelectNode, Graph);

	return SelectNode;
}

UK2Node* FUtilityNodeCreator::CreateSpawnActorNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	UK2Node_SpawnActorFromClass* SpawnActorNode = NewObject<UK2Node_SpawnActorFromClass>(Graph);
	if (!SpawnActorNode)
	{
		return nullptr;
	}

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	SpawnActorNode->NodePosX = static_cast<int32>(PosX);
	SpawnActorNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(SpawnActorNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(SpawnActorNode, Graph);

	return SpawnActorNode;
}

