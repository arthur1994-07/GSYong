#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharData.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;

class CLunchBoxTimeDisplay : public CUIGroup
{
private:
	enum
	{
		LUNCHBOXTIME_DUMMY_BASE = NO_ID + 1,
		LUNCHBOXTIME_DUMMY_SIZE = 5,
		LUNCHBOXTIME_UNIT = 1000,
		LUNCHBOXTIME_UNIT_END = LUNCHBOXTIME_UNIT + 10000,
	};

public:
	CLunchBoxTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CLunchBoxTimeDisplay ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();
	
public:
	void	SetEnableAll( const BOOL bEnable );

private:
	bool	NEW_PLAY_LUNCHBOX( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot );
	UIGUID	GET_EXIST_PLAY_LUNCHBOX_CONTROL( const DWORD& dwSkillID );
	bool	GET_PLAY_LUNCHBOX( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT );

private:
	bool	GET_LUNCHBOX_LIFE_TIME( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime );

public:
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	typedef	std::pair<DWORD,UIGUID>				LUNCHBOXTIME_PAIR;
	typedef	std::list<LUNCHBOXTIME_PAIR>		LUNCHBOXTIME_DISPLAY_LIST;
	typedef	LUNCHBOXTIME_DISPLAY_LIST::iterator	LUNCHBOXTIME_DISPLAY_LIST_ITER;

	LUNCHBOXTIME_DISPLAY_LIST	m_LunchboxPlayList;
	UIGUID						m_NEWID;

	LPDIRECT3DDEVICEQ			m_pd3dDevice;

private:
	CUIControl* m_pLunchBoxUnitDummy[SKILLFACT_SIZE_LUNCHBOX + SKILLFACT_SIZE_EXP_LUNCHBOX];

protected:
	CInnerInterface* m_pInterface;

};

class MyLunchBoxTimeDisplay : public ILunchBoxTimeDisplay, private CLunchBoxTimeDisplay
{
public:
    MyLunchBoxTimeDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyLunchBoxTimeDisplay()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
};