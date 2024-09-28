#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharData.h"

class CInnerInterface;
class GLGaeaClient;

class CSkillTimeDisplay : public CUIGroup
{
private:
	enum
	{
		SKILLTIME_DUMMY_BASE = NO_ID + 1,
		SKILLTIME_DUMMY_SIZE = 5,
		SKILLTIME_UNIT = 1000,
		SKILLTIME_UNIT_END = SKILLTIME_UNIT + 10000,
	};

public:
	CSkillTimeDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSkillTimeDisplay ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void		CreateSubControl ();

private:
	bool		NEW_PLAY_ADDITIONAL( const DWORD dwOption, const float fTimeLength );
	bool		NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL, const WORD wSlot );
	UIGUID		GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID );
	bool		GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT );

private:
	bool		GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime );
	void		RegisterPlaySkill( EMSKILLFACT_TYPE emType );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	typedef	std::pair<DWORD,UIGUID>			SKILLTIME_PAIR;
	typedef	std::list<SKILLTIME_PAIR>		SKILLTIME_DISPLAY_LIST;
	typedef	SKILLTIME_DISPLAY_LIST::iterator	SKILLTIME_DISPLAY_LIST_ITER;

	SKILLTIME_DISPLAY_LIST	m_SkillPlayList;
	UIGUID	m_NEWID;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

private:
	CUIControl*		m_pSkillUnitDummy[SKILLFACT_SIZE_NORMAL_INC_ADDITIONAL];

protected:
	CInnerInterface* m_pInterface;

};