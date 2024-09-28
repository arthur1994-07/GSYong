#pragma	once
#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CSkillImage;
class CBasicLineBoxSmart;
class CBasicTextBox;

class RnCompetitionRewardBuff : public CUIGroup
{
public:
	RnCompetitionRewardBuff( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnCompetitionRewardBuff();
public:
	CSkillImage* m_pSkillImage;

protected:
	GLGaeaClient * m_pGaeaClient;
	CInnerInterface* m_pInterface;

public:
	void CreateSubControl ();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nscroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void SetSkill( SNATIVEID sNativeID );

};
