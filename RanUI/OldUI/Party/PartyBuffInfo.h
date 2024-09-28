#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Character/GLCharData.h"

class CInnerInterface;
class CBasicLineBox;
class GLGaeaClient;
struct GLPartySlotClient;

class CPartyBuffInfo : public CUIGroup
{
private:
	enum
	{
		SKILLTIME_UNIT = 1000,
		SKILLTIME_UNIT_END = SKILLTIME_UNIT + 1000,
	};

public:
	CPartyBuffInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPartyBuffInfo ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();
	void	SetPartyID(const DWORD nMemberIndex, const DWORD nPartyIndex = MASTER_PARTY);

private:
	bool	NEW_PLAY_SKILL ( const DWORD& dwSkillID, const WORD wLEVEL );
	UIGUID	GET_EXIST_PLAY_SKILL_CONTROL ( const DWORD& dwSkillID );
	bool	GET_PLAY_SKILLFACT ( const DWORD& dwSkillID, SSKILLFACT& sSKILLFACT );

private:
	bool	GET_SKILL_LIFE_TIME ( const DWORD& dwSkillID, const WORD wLEVEL, float& fLifeTime );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

private:
	typedef	std::pair<DWORD,UIGUID>			SKILLTIME_PAIR;
	typedef	std::list<SKILLTIME_PAIR>		SKILLTIME_DISPLAY_LIST;
	typedef	SKILLTIME_DISPLAY_LIST::iterator	SKILLTIME_DISPLAY_LIST_ITER;

	SKILLTIME_DISPLAY_LIST	m_SkillPlayList;
	UIGUID	m_NEWID;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

private:
	GLPartySlotClient* const FindSelfClient(void);

private:
	CUIControl*		m_pSkillUnitDummy[SKILLFACT_SIZE_NORMAL];
	DWORD			m_nMemberIndex;
	DWORD			m_nPartyIndex;

	CBasicLineBox*	m_pLineBox;

	float	m_fBUFF_SIZE_X;
	float	m_fBUFF_BACK_SIZE_X;

private:
	CInnerInterface* m_pInterface;
};