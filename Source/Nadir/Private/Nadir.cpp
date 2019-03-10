// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Nadir.h"
#include "NadirEdMode.h"
#include "Interfaces/IPluginManager.h"

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

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("Nadir")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/exampleLib/ExampleLib.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FoolLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS

	void* ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		//ExampleLibraryFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FNadirModule::Unregister()
{
	FEditorModeRegistry::Get().UnregisterMode(FNadirEdMode::EM_NadirEdModeId);

	//FMeshEditorStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNadirModule, Nadir)