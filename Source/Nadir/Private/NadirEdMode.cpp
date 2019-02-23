// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NadirEdMode.h"
#include "NadirEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FNadirEdMode::EM_NadirEdModeId = TEXT("EM_NadirEdMode");

FNadirEdMode::FNadirEdMode()
{

}

FNadirEdMode::~FNadirEdMode()
{

}

void FNadirEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FNadirEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FNadirEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FNadirEdMode::UsesToolkits() const
{
	return true;
}




