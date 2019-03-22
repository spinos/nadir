// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Nadir.h"
#include "NadirEdMode.h"
#include "NadirEdModeStyle.h"
#include "Interfaces/IPluginManager.h"
#include <string>

#define LOCTEXT_NAMESPACE "FNadirModule"

typedef bool(*_exampleLibraryTestHdf)(const std::string &filename);
_exampleLibraryTestHdf _getExampleLibraryTestHdfFromDll;

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
	FNadirEdModeStyle::Initialize();

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("Nadir")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath, hdf5LibPath, hdf5cppLibPath, zlibPath, szipPath;
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
		_getExampleLibraryTestHdfFromDll = nullptr;
		FString procName = "ExampleLibraryTestHdf";	// Needs to be the exact name of the DLL method.
		_getExampleLibraryTestHdfFromDll = (_exampleLibraryTestHdf)FPlatformProcess::GetDllExport(ExampleLibraryHandle, *procName);
		if (_getExampleLibraryTestHdfFromDll != NULL)
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryInfo", "example library has function ExampleLibraryTestHdf "));
			std::string hdfFileName("D:/bar.hes");
			bool isHdfSaved = bool(_getExampleLibraryTestHdfFromDll(hdfFileName));
		
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}

	FEditorModeRegistry::Get().RegisterMode<FNadirEdMode>(FNadirEdMode::EM_NadirEdModeId, 
		LOCTEXT("NadirEdModeName", "NadirEditMode"), 
		FSlateIcon(FNadirEdModeStyle::GetStyleSetName(), "NadirEditor.Tab" ),
		true);

}

void FNadirModule::Unregister()
{
	FEditorModeRegistry::Get().UnregisterMode(FNadirEdMode::EM_NadirEdModeId);

	FNadirEdModeStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNadirModule, Nadir)