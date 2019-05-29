// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class SGridPanel;
class SScrollBox;
class SMultiLineEditableTextBox;
class SButton;

class FNadirEdModeToolkit : public FModeToolkit
{
	enum TabRadioChoice {
		TabRadioWrite = 0,
		TabRadioOpen
	};

	TabRadioChoice m_selectedTab;
	
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

	TSharedRef<SGridPanel> makeRadioPanel();
	ECheckBoxState handleTabRadioIsChecked( TabRadioChoice ButtonId ) const;
	void handleTabRadioCheckStateChanged( ECheckBoxState NewRadioState, TabRadioChoice RadioThatChanged );

	TSharedRef<SScrollBox> makeWritePanel();
	TSharedRef<SScrollBox> makeReadPanel();

private:

	TSharedPtr<SWidget> ToolkitWidget;
	TSharedPtr<SWidgetSwitcher> m_panelSelector;
	TSharedPtr<SMultiLineEditableTextBox> m_miscStatsField;
	TSharedPtr<SButton> m_saveActBtn;
};

DECLARE_LOG_CATEGORY_EXTERN(LogNadir, Log, All);
