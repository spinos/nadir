/*
 *  NadirEdModeToolkit.h
 *
 *  2019/5/30
 */

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FNadirEdModeToolkit : public FModeToolkit
{

public:

	FNadirEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }

private:

	TSharedPtr<SWidget> ToolkitWidget;
	
};

DECLARE_LOG_CATEGORY_EXTERN(LogNadir, Log, All);
