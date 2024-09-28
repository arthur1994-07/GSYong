
#pragma	once

#include "./BasicComboBoxRollOver.h"
#include "UITextControl.h"

class	CSmartComboBoxRollOver : public CBasicComboBoxRollOver
{
public:
	CSmartComboBoxRollOver (EngineDeviceMan* pEngineDevice);
	virtual	~CSmartComboBoxRollOver ();

public:
    void CreateSmartComboBoxRollOver(
        const char* szComboBoxControl,
        const char* szComboBoxLineInfoControl,
        bool bSelectInit=TRUE);

private:
    void DoCreateSmartComboBoxRollOver(
        const char* szComboBoxControl,
        const char* szComboBoxLineInfoControl,
        bool bSelectInit,
        const UIRECT& rcWorkSpace );

public:
    int AddTextNoSplit(
        const CString& str,
        bool bVisibleScroll=FALSE,
        const D3DCOLOR& TextColor=NS_UITEXTCOLOR::WHITE);

    void SetMouseOverColor(const D3DCOLOR cColor);

public:
    void ResizeToTextSize ( bool bVisibleScroll );

protected:
	virtual void CreateBaseComboBoxRollOver(
        char* szComboBoxControl,
        bool bSelectInit = TRUE ) {}
	virtual void CreateComboBoxRollOverImage ( bool bSelectInit )								 {}
	virtual void CreateScrollBar ()																 {}

private:
    UIRECT m_rcWorkSpace;
};
