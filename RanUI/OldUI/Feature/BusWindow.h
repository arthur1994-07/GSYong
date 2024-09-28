#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"
#include "../../../RanLogic/Transport/GLBusList.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;
class	CBasicScrollBarEx;
class	CBasicButton;
class	GLGaeaClient;

struct	SNpcTalk;

class	CBusWindow 
    : public  CUIWindowEx
    , private CUIControlNpcActionable
{
private:
	enum
	{
		BUS_MAP_LIST = ET_CONTROL_NEXT,
		BUS_MAP_SCROLLBAR,
		BUS_STOP_LIST,
		BUS_STOP_SCROLLBAR,
		BUS_MOVEBUTTON,
	};

public:
	CBusWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CBusWindow ();

public:
	void	CreateSubControl ();	

	bool SetBusData(
        const SNATIVEID& sNPCID,
        const DWORD dwNPCGlobalID,
        std::tr1::shared_ptr<SNpcTalk> spNpcTalk,
        bool bToolMode);

private:
	void	LoadMapList ();
	void	LoadStopList ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

public:
    SNATIVEID    GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD        GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

private:
	int		m_nSelectMap;
	int		m_nSelectStop;

private:
	CBasicTextBox*		m_pMapTitle;
	CBasicTextBox*		m_pStopTitle;

	CBasicTextBoxEx*	m_pMapList;
	CBasicScrollBarEx*	m_pMapListScrollBar;

	CBasicTextBoxEx*	m_pStopList;
	CBasicScrollBarEx*	m_pStopListScrollBar;

	CBasicButton*		m_pMoveButton;

private:
	std::tr1::shared_ptr<SNpcTalk> m_spNpcTalk;
	GLBusList	m_glBusList;

protected:
	GLGaeaClient* m_pGaeaClient;

};
