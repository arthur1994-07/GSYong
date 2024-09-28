#pragma once

#include "../../../../../../InnerInterface.h"
#include "../../../../../Util/UIGroupHelper.h"

class GLGaeaClient;
struct GLPartySlotClient;
class ExpeditionInfoPartySlot : public CUIGroupHelper
{	
public:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );	

	const bool isValid(void) const;
	const DWORD GetGaeaID(void) const;
	const CString& GetName(void) const;

	void SetInformation( std::tr1::shared_ptr<GLCharClient> pPartySlot);
	void SetInformation(const GLPartySlotClient* const pPartySlot);
	void SetMasterInformation(const GLPartySlotClient* const pPartySlot, const bool bMaster);
	void SetSelect(void);
	void SetBlank(const bool bOn);

	void ResetSelect(void);

	void CreateSubControl(void);

	void RESET(void);

public:
	ExpeditionInfoPartySlot(EngineDeviceMan* pEngineDevice, const DWORD dwMemberSlot);

private:
	CUIControl* m_pSelectBox; // ���� �ڽ�;
	CUIControl* m_pMasterCrown[2]; // ��Ƽ/������(��)�� �հ� ��ũ;
	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // �п� ��ũ;
	CUIControl* m_pCheckReadyOk; // �غ� Ȯ�� ��ũ;
	CUIControl* m_pClassBox[GLCL_NUM_CLASS]; // Ŭ���� ����;
	CBasicTextBox* m_pLevel; // ����;
	CBasicTextBox* m_pCharacterName; // �ɸ��� �̸�;	

	const DWORD m_dwMemberSlot;
	DWORD m_dwGaeaID;
	CString m_szName;	
};