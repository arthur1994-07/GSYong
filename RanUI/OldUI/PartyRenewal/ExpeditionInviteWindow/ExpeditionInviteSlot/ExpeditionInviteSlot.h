#pragma once

#include "../../../../InnerInterface.h"
#include "../../../Util/UIGroupHelper.h"

class CUIControl;
class CBasicScrollBarEx;
class CBasicTextBox;

class GLGaeaClient;
class ExpeditionInviteSlot : public CUIGroupHelper
{
public:
	void CreateSubControl(void);

	void SetSlot(const char* const szName, const DWORD dwDbNum, const bool bSelected);
	void Refresh(const bool bSelected);

public:
	ExpeditionInviteSlot(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~ExpeditionInviteSlot(void);

private:
	GLGaeaClient* m_pGaeaClient;
	
	CBasicTextBox* m_pCharacterName; // �ɸ��� �̸�;
	CUIControl* m_pSelectBox; // ���� ����;

	DWORD m_dwDbNum;
};