#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CMiniPartySlot;
class CPartyBuffInfo;
class GLGaeaClient;
struct GLPartySlotClient;

class CMiniPartyWindow : public CUIWindowEx
{
protected:
	enum
	{
		MINIPARTY_SLOT_MEMBER0 = ET_CONTROL_NEXT,
		MINIPARTY_SLOT_MEMBER1,
		MINIPARTY_SLOT_MEMBER2,
		MINIPARTY_SLOT_MEMBER3,
		MINIPARTY_SLOT_MEMBER4,
		MINIPARTY_SLOT_MEMBER5,
		MINIPARTY_SLOT_MEMBER6,
		MINIPARTY_SLOT_MEMBER7,

		MINIPARTY_BUFF_INFO,
	};

public:
	CMiniPartyWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CMiniPartyWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();

private:
	GLPartySlotClient* const FindSelfClient(void);
	BOOL	IsSameMap ( GLPartySlotClient *pSelf, GLPartySlotClient *pMember );

public:
	virtual void SetSlotMember ( CMiniPartySlot* pPartySlot, GLPartySlotClient *pMember, BOOL bSameMap, const int nPartySlotID );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	CMiniPartySlot*		m_pPartySlot[MAXPARTY];
	CUIControl*			m_pSlotOver[MAXPARTY];
	CPartyBuffInfo*		m_pPartyBuffInfo;
	bool				m_bMOUSEIN;

};