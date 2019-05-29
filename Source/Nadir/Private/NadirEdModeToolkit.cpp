/*
 *  NadirEdModeToolkit.cpp
 *
 *  2019/5/30
 */

#include "NadirEdModeToolkit.h"
#include "NadirEdMode.h"
#include "NadirEdModeWidget.h"

DEFINE_LOG_CATEGORY(LogNadir);

#define LOCTEXT_NAMESPACE "FNadirEdModeToolkit"

FNadirEdModeToolkit::FNadirEdModeToolkit()
{
}

void FNadirEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	SAssignNew(ToolkitWidget, NadirEdModeWidget);
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FNadirEdModeToolkit::GetToolkitFName() const
{
	return FName("NadirEdMode");
}

FText FNadirEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("NadirEdModeToolkit", "DisplayName", "Nadir Tool");
}

class FEdMode* FNadirEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FNadirEdMode::EM_NadirEdModeId);
}

#undef LOCTEXT_NAMESPACE
