#pragma	once
#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CSkillImage;
class CBasicLineBoxSmart;
class CItemImageEx;

class RnCompetitionRewardItem : public CUIGroup
{
public:
	RnCompetitionRewardItem( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~RnCompetitionRewardItem();
public:
	CItemImageEx* m_pItemImage;
	CBasicTextBox* m_pTurnNum;

protected:
	GLGaeaClient * m_pGaeaClient;
	CInnerInterface* m_pInterface;

public:
	void CreateSubControl ();
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nscroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void SetItemId( SNATIVEID sNativeID );
	void SetTurnNum ( const WORD wTurnNum );
};
