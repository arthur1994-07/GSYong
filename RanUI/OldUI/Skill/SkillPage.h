#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Skill/GLSkill.h"

class CInnerInterface;
class CSkillSlot;
class CBasicScrollBarEx;
class GLGaeaClient;

class CSkillPage : public CUIGroup
{
protected:
static	const	int		nSTARTLINE;
static	const	int		nOUTOFRANGE;

protected:
	enum
	{
		SKILL_SLOT0 = NO_ID + 1,
		SKILL_SLOT1,
		SKILL_SLOT2,
		SKILL_SLOT3,
		SKILL_SLOT4,
		SKILL_SLOT5,
		SKILL_SLOT6,
		SKILL_SLOT7,
		SKILL_SLOT_END = SKILL_SLOT0 + GLSkillMan::MAX_CLASSSKILL,
	};

	enum
	{
		nMAX_ONE_VIEW_SLOT = 8
	};

public:
	CSkillPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillPage();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ( SNATIVEID sNativeIDArray[GLSkillMan::MAX_CLASSSKILL], int nSkillCount );	

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	void	ResetAllSkillSlotRender ( int nTotal );
	void	SetSkillSlotRender ( int nStartIndex, int nTotal );

public:
	int			GetSkillIndex ()						{ return m_nSlotIndex; }
	SNATIVEID	GetSkillID ( int nIndex );
	BOOL		GetViewSkillSlot ( int nIndex );

protected:
	CUIControl*	CreateControl ( const char* szControl );
	CSkillSlot*	CreateSkillSlot ( CString strKeyword, UIGUID ControlID, SNATIVEID sNativeID );
public:
	CSkillSlot*	m_pSkillSlotArray[GLSkillMan::MAX_CLASSSKILL];
private:	
	CUIControl*	m_pSkillSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

private:
	CBasicScrollBarEx*	m_pScrollBar;

private:
	int		 m_nSlotIndex;
	int		 m_nCurPos;
	int		 m_nMaxSkillCnt;
	std::vector<int> m_vecViewSkill;
	std::vector<int> m_vecNotViewSkill;

protected:
	CInnerInterface* m_pInterface;

};