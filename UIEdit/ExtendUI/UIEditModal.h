#pragma	once

#include "./UIEditGroup.h"

class CUIEditModal : public CUIEditGroup
{

public:
	CUIEditModal(EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin);
	virtual	~CUIEditModal ();

public:
	CUIControl*	 CreateControl ( TCHAR* szControl, WORD wAlignFlag = UI_FLAG_DEFAULT );

	virtual void CreateWindowControl (const TCHAR* szControl);
	virtual void ResizeControl(const TCHAR* szControl);
	virtual void ResizeWindow(const UIRECT& rcNew) override;


public:
	CUIControl*		m_pLeft;
	CUIControl*		m_pRight;
	CUIControl*		m_pTop;
	CUIControl*		m_pBottom;
	CUIControl*		m_pLeftTop;
	CUIControl*		m_pRightTop;
	CUIControl*		m_pLeftBottom;
	CUIControl*		m_pRightBottom;
	CUIControl*		m_pCenter;
};
