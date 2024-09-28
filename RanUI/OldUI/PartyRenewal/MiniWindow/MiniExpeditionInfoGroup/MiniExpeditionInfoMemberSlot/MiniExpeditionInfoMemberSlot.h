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

	CUIControl* m_pMasterCrown[2]; // 파티/원정대(부)장 왕관 마크;
	CUIControl* m_pSelectBox; // 선택 박스;
	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // 학원 마크;
	CUIControl* m_pCheckReadyOk; // 준비 확인 마크;
	CUIControl* m_pClassBox[GLCL_NUM_CLASS]; // 직업 색상 배경;
	CBasicTextBox* m_pLevel; // 레벨;
	CBasicTextBox* m_pCharacterName; // 케릭터 이름;	
protected:
	CString m_szName;
	const DWORD m_dwPartySlot;
	DWORD m_dwGaeaID;	

private:
	

	

	
};