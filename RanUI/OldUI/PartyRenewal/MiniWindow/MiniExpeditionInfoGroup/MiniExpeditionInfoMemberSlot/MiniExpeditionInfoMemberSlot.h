#pragma once

#include "../../../../../InnerInterface.h"
#include "../../../../Util/UIGroupHelper.h"

class GLGaeaClient;
struct GLPartySlotClient;
class MiniExpeditionInfoPartySlot : public CUIGroupHelper
{	
public:
	const bool isValid(void) const;
	const DWORD GetGaeaID(void) const;
	const CString& GetName(void) const;

	void SetInformation(const GLPartySlotClient* const pPartySlot);
	void SetMasterInformation(const GLPartySlotClient* const pPartySlot, const bool bMaster);

	void SetSelect(void);

	void CreateSubControl(void);

	void RESET(void);

public:
	MiniExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwPartySlot);

public:

	CUIControl* m_pMasterCrown[2]; // ��Ƽ/������(��)�� �հ� ��ũ;
	CUIControl* m_pSelectBox; // ���� �ڽ�;
	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // �п� ��ũ;
	CUIControl* m_pCheckReadyOk; // �غ� Ȯ�� ��ũ;
	CUIControl* m_pClassBox[GLCL_NUM_CLASS]; // ���� ���� ���;
	CBasicTextBox* m_pLevel; // ����;
	CBasicTextBox* m_pCharacterName; // �ɸ��� �̸�;	
protected:
	CString m_szName;
	const DWORD m_dwPartySlot;
	DWORD m_dwGaeaID;	

private:
	

	

	
};