#pragma once

#include "../../../../../../RanLogic/Character/GLCharDefine.h"
#include "../../../../Util/UIGroupHelper.h"

class CUIControl;
class CBasicTextBox;
class CBasicProgressBar;

class ClientActor;
class GLGaeaClient;
struct GLPartySlotClient;
class MemberStateSlotInformation : public CUIGroupHelper
{
public:
	void CreateSubControl(void);	
	void SetInformation(const GLPartySlotClient* const pPartyClient, const bool bAvailable);
	void SetInformation(GLCharacter* pChar, const bool bAvailable);
	void SetInformation(std::tr1::shared_ptr<GLCharClient> pPartyClient, const bool bAvailable);

public:
	MemberStateSlotInformation(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~MemberStateSlotInformation(void){}

private:
	GLGaeaClient* m_pGaeaClient;
	CUIControl* m_pSchoolMark[SCHOOL_NUM]; // 학원 마크;
	CBasicTextBox* m_pCharacterName; // 케릭터 이름;
	CBasicTextBox* m_pHPText; // 케릭터 체력;
	CBasicProgressBar* m_pHPProgressBar[GLCL_NUM_CLASS]; // 체력 막대;
};

