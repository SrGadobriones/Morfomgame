#include "Commands/BlueprintGraph/Nodes/DataNodes.h"
#include "Commands/BlueprintGraph/Nodes/NodeCreatorUtils.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_MakeArray.h"
#include "Json.h"

UK2Node* FDataNodeCreator::CreateVariableGetNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	FString VariableName;
	if (!Params->TryGetStringField(TEXT("variable_name"), VariableName))
	{
		return nullptr;
	}

	UK2Node_VariableGet* VarGetNode = NewObject<UK2Node_VariableGet>(Graph);
	if (!VarGetNode)
	{
		return nullptr;
	}

	// Optional external class: sub-property on a component / external class
	FString VarClassStr;
	bool bSetExternal = false;
	if (Params->TryGetStringField(TEXT("variable_class"), VarClassStr) && !VarClassStr.IsEmpty())
	{
		UClass* VarCls = nullptr;
		if (VarClassStr.StartsWith(TEXT("/"))) VarCls = LoadObject<UClass>(nullptr, *VarClassStr);
		if (!VarCls) VarCls = UClass::TryFindTypeSlow<UClass>(VarClassStr);
		if (!VarCls)
		{
			static const TCHAR* Prefixes[] = { TEXT("/Script/Engine."), TEXT("/Script/UMG."), TEXT("/Script/CoreUObject."), TEXT("/Script/EnhancedInput.") };
			for (const TCHAR* P : Prefixes) { VarCls = LoadObject<UClass>(nullptr, *(FString(P) + VarClassStr)); if (VarCls) break; }
		}
		if (VarCls)
		{
			VarGetNode->VariableReference.SetExternalMember(FName(*VariableName), VarCls);
			bSetExternal = true;
		}
	}
	if (!bSetExternal) VarGetNode->VariableReference.SetSelfMember(FName(*VariableName));

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	VarGetNode->NodePosX = static_cast<int32>(PosX);
	VarGetNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(VarGetNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(VarGetNode, Graph);

	return VarGetNode;
}

UK2Node* FDataNodeCreator::CreateVariableSetNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	FString VariableName;
	if (!Params->TryGetStringField(TEXT("variable_name"), VariableName))
	{
		return nullptr;
	}

	UK2Node_VariableSet* VarSetNode = NewObject<UK2Node_VariableSet>(Graph);
	if (!VarSetNode)
	{
		return nullptr;
	}

	{
		FString VarClassStr;
		bool bSetExternalS = false;
		if (Params->TryGetStringField(TEXT("variable_class"), VarClassStr) && !VarClassStr.IsEmpty())
		{
			UClass* VarCls = nullptr;
			if (VarClassStr.StartsWith(TEXT("/"))) VarCls = LoadObject<UClass>(nullptr, *VarClassStr);
			if (!VarCls) VarCls = UClass::TryFindTypeSlow<UClass>(VarClassStr);
			if (!VarCls)
			{
				static const TCHAR* Prefixes[] = { TEXT("/Script/Engine."), TEXT("/Script/UMG."), TEXT("/Script/CoreUObject."), TEXT("/Script/EnhancedInput.") };
				for (const TCHAR* P : Prefixes) { VarCls = LoadObject<UClass>(nullptr, *(FString(P) + VarClassStr)); if (VarCls) break; }
			}
			if (VarCls)
			{
				VarSetNode->VariableReference.SetExternalMember(FName(*VariableName), VarCls);
				bSetExternalS = true;
			}
		}
		if (!bSetExternalS) VarSetNode->VariableReference.SetSelfMember(FName(*VariableName));
	}

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	VarSetNode->NodePosX = static_cast<int32>(PosX);
	VarSetNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(VarSetNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(VarSetNode, Graph);

	return VarSetNode;
}


UK2Node* FDataNodeCreator::CreateMakeArrayNode(UEdGraph* Graph, const TSharedPtr<FJsonObject>& Params)
{
	if (!Graph || !Params.IsValid())
	{
		return nullptr;
	}

	UK2Node_MakeArray* MakeArrayNode = NewObject<UK2Node_MakeArray>(Graph);
	if (!MakeArrayNode)
	{
		return nullptr;
	}

	double PosX, PosY;
	FNodeCreatorUtils::ExtractNodePosition(Params, PosX, PosY);
	MakeArrayNode->NodePosX = static_cast<int32>(PosX);
	MakeArrayNode->NodePosY = static_cast<int32>(PosY);

	Graph->AddNode(MakeArrayNode, true, false);
	FNodeCreatorUtils::InitializeK2Node(MakeArrayNode, Graph);

	return MakeArrayNode;
}

