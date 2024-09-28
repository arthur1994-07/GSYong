#pragma	once

#include "BasicQuickSkillSlot.h"
#include "SkillTrayTab.h"

class CInnerInterface;
class CBasicTextBox;

class CBasicQuickSkillSlotEx : public CBasicQuickSkillSlot
{
private:
static	const	int		nNOT_INIT;

public:
	CBasicQuickSkillSlotEx(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicQuickSkillSlotEx ();

public:
	virtual	void CreateSubControl ( INT nTabIndex );
	void	CreateNumberText ( CD3DFontPar* pFont8, int nNumber );

	INT GetTabIndex()			{ return m_nTabIndex; }
	void	SetShotcutText( CString strTemp );

public:
	virtual void ResetText ();

public:
    virtual void TurnHorizontal ();
    virtual void TurnVertical   ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );	

public:
	void SetVisibleReservedSkill( const bool bEnable );
	void SetReservedSkillOrder( const UINT nOrder, const UINT nMax );

private:
	int					m_nNumber;
	CBasicTextBox*		m_pNumberText;
	CUIControl*			m_pReserved;
	INT					m_nTabIndex;

protected:
	CInnerInterface* m_pInterface;

};