// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Nadir.h"
#include "NadirEdMode.h"

#define LOCTEXT_NAMESPACE "FNadirModule"

FNadirModule::FNadirModule() : m_isEnabled(false)
{}

void FNadirModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Register();
}

void FNadirModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	Unregister();
}

void FNadirModule::Register()
{
	//FMeshEditorStyle::Initialize();

	FEditorModeRegistry::Get().RegisterMode<FNadirEdMode>(FNadirEdMode::EM_NadirEdModeId, 
		LOCTEXT("NadirEdModeName", "NadirEditMode"), 
		FSlateIcon(), 
		true);
}

void FNadirModule::Unregister()
{
	FEditorModeRegistry::Get().UnregisterMode(FNadirEdMode::EM_NadirEdModeId);

	//FMeshEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNadirModule, Nadir)