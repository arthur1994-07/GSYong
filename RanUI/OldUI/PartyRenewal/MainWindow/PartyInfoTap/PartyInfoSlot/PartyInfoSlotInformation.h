#pragma once

#include "../../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../../Util/UIGroupHelper.h"

class CUIControl;
class CBasicTextBox;

class GLGaeaClient;
class ClientActor;
struct GLPartySlotClient;
class PartyInfoSlotInformation : public CUIGroupHelper
{
public:
	void CreateSubControl(void);
	void SetInformation(const GLPartySlotClient* const pPartyClient, const bool bAvailable);
	void SetColor(const DWORD dwColor);

	const DWORD GetGaeaID(void) const;
	const CString& GetName(void) const;

	void RESET(void);

public:
	PartyInfoSlotInformation(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~PartyInfoSlotInformation(void){}

private:
	GLGaeaClient* m_pGaeaClient;

	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // 학원 마크;
	CBasicTextBox* m_pLevel; // 레벨;
	CBasicTextBox* m_pCharacterName; // 케릭터 이름;
	CBasicTextBox* m_pLoacation; // 장소(이름);
	CBasicTextBox* m_pPosition; // 위치(좌표);

	DWORD m_dwGaeaID;
	DWORD m_dwClubDbNum;
	DWORD m_dwClubMarkVersion;
	CString m_szName;
};