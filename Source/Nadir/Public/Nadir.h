// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorModeRegistry.h"
#include "Modules/ModuleManager.h"

class FNadirModule : public IModuleInterface
{
	bool m_isEnabled;

public:

	FNadirModule();

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void Register();
	void Unregister();

private:
	void *m_hdf5LibraryHandle;

};
