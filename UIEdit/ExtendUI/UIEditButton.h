#pragma	once

#include "./UIEditGroup.h"

class CBasicLineBox;

class CUIEditButton : public CUIEditGroup
{
public:
	CUIEditButton(EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin);
	virtual	~CUIEditButton();

public:
	virtual void ResizeWindow(const UIRECT& rcNew) override;
	virtual void CreateWindowControl (const TCHAR* szControl);
	virtual void CheckMousestate();

public:
	CUIControl*		m_pMouseInFlip;
	CUIControl*		m_pMouseClickFlip;
};
