
#pragma once

#include "../../EngineLib/GUInterface/UIControlMessage.h"
class CUIGroup;

namespace SETITEM_UIMSG_DEFINE{
	const DWORD dwSETITEM_UIMSG_CHECK_MOUSE_IN = UIMSG_USER1;
}


class CSetitemPreviewlistSlot : public CUIGroupHelper
{
private:
	enum { IDD_NAMEBOX = NO_ID+1, };
	DWORD				m_dwSlotIndex;

public:
	CSetitemPreviewlistSlot(EngineDeviceMan* pEngineDevice, const DWORD nSlotIndex);
	virtual ~CSetitemPreviewlistSlot(void) {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

public:
	void SetKeySlotInfo(const DWORD dwSlot, const std::string strItemName, const BOOL bWoreSetitem);
	const DWORD GetSlotIndex(void) const	{ return m_dwSlotIndex; }
};