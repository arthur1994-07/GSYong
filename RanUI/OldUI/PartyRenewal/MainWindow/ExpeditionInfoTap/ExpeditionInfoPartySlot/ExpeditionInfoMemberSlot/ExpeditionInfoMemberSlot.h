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
	CUIControl* m_pSelectBox; // 선택 박스;
	CUIControl* m_pMasterCrown[2]; // 파티/원정대(부)장 왕관 마크;
	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // 학원 마크;
	CUIControl* m_pCheckReadyOk; // 준비 확인 마크;
	CUIControl* m_pClassBox[GLCL_NUM_CLASS]; // 클래스 색상;
	CBasicTextBox* m_pLevel; // 레벨;
	CBasicTextBox* m_pCharacterName; // 케릭터 이름;	

	const DWORD m_dwMemberSlot;
	DWORD m_dwGaeaID;
	CString m_szName;	
};