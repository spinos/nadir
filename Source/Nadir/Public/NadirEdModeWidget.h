/*
 *  NadirEdModeWidget.h
 *
 *  https://wiki.unrealengine.com/Slate,_Hello
 *
 *  2019/5/30
 */

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SGridPanel;
class SScrollBox;
class SMultiLineEditableTextBox;
class SButton;

class NadirEdModeWidget : public SCompoundWidget
{
	enum TabRadioChoice {
		TabRadioWrite = 0,
		TabRadioOpen
	};

	TabRadioChoice m_selectedTab;

public:
	SLATE_BEGIN_ARGS(NadirEdModeWidget) {}
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

protected:

private:
	TSharedRef<SGridPanel> makeRadioPanel();
	TSharedRef<SWidgetSwitcher> makeMainPanel();
	
	FReply OnButtonSaveClick();
	FReply OnButtonSendClick();
	FReply OnButtonTestRotationClick();
	FReply OnButtonCreateMeshClick();
	FReply OnButtonCreateActorClick();
	FReply OnButtonTestAnimationClick();
	void resetStatsField();

	ECheckBoxState handleTabRadioIsChecked( TabRadioChoice ButtonId ) const;
	void handleTabRadioCheckStateChanged( ECheckBoxState NewRadioState, TabRadioChoice RadioThatChanged );

	TSharedRef<SScrollBox> makeWritePanel();
	TSharedRef<SScrollBox> makeReadPanel();

	TSharedPtr<SWidgetSwitcher> m_panelSelector;
	TSharedPtr<SMultiLineEditableTextBox> m_miscStatsField;
	TSharedPtr<SButton> m_saveActBtn;

};
