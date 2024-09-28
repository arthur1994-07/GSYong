#pragma once

#include "SkillInforTooltip.h"
#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CItemImage;
class CBasicTextBox;
class CBasicVarTextBox;
class CSkillImage;
struct GLCHARLOGIC;

class CSkillLinkInfoTooltip : public CUIGroup
{
public:
	CSkillLinkInfoTooltip( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CSkillLinkInfoTooltip();

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CSkillInfoTooltip* m_pLinkSkillTooltip[SKILL::MAX_ADDON];
	WORD m_wLinkCount;

private:
	void ResetCount(){ m_wLinkCount = 0; }
public:
	void CreateSubControl();
	void SetBGAlpha(int nValue);
	CSkillInfoTooltip* GetSkillInfoTooltip(){ return m_pLinkSkillTooltip[0]; }

	void SetDealyActionSSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void SetOtherMultiSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void SetMainSkillTooltip( const CSkillInfoTooltip* pSkillTooltip );
	void DisplaySkillInformation( GLCHARLOGIC* pTarget, const GLSKILL* pSkill, const WORD wShowLevel, const WORD _emTooltipType = CSkillInfoTooltip::BASIC_TOOLTIP, const bool bShowReq = false );

	void Reset();

	bool IsHaveSkill( SNATIVEID sNativeID );
public:
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( bool bVisible );
	void TranslateMessage( DWORD dwMsg );
};
