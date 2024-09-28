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

	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // �п� ��ũ;
	CBasicTextBox* m_pLevel; // ����;
	CBasicTextBox* m_pCharacterName; // �ɸ��� �̸�;
	CBasicTextBox* m_pLoacation; // ���(�̸�);
	CBasicTextBox* m_pPosition; // ��ġ(��ǥ);

	DWORD m_dwGaeaID;
	DWORD m_dwClubDbNum;
	DWORD m_dwClubMarkVersion;
	CString m_szName;
};