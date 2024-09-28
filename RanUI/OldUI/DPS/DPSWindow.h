#pragma once

#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"

class GLGaeaClient;
class DPSPage;
class DetailPage;
class DPSWindow : public CUIWindowEx, IDPSWindow
{	
public:	
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

private:
	void CreateSubControl(void);

public:
	DPSWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~DPSWindow(void){};

private:
	GLGaeaClient* const m_pGaeaClient;
	DPSPage* m_pDPSPage;
	DetailPage* m_pDetailPage;
//	DetailSlot* m_pDetailSlot[DamageRecord::VIEW_SLOT_NSIZE + 1]; // +1 : ≈∏¿Ã∆≤ ΩΩ∑‘;	
	float m_fElapsedTime;
};