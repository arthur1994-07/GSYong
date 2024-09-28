#pragma	once

#include "../Util/UIOuterWindow.h"
//#include "../../../RanLogic/s_NetGlobal.h"

class	COuterInterface;
class	CBasicTextBox;
class	CBasicTextBoxEx;

const DWORD UIMSG_MOUSEIN_LOGIN = UIMSG_USER1;

class CSelectServerChannel : public CUIOuterWindow
{
private:
static	const	float		fLIST_PART_INTERVAL;
static	const	D3DCOLOR	dwCLICKCOLOR;
static	const	int			nOUTOFRANGE;
static	const	float		fNORMAL_PERCENT;
static	const	float		fMAX_PERCENT;

private:
	enum
	{
		SELECT_SERVER_CONNECT = ET_CONTROL_NEXT,		
		SELECT_SERVER_LIST,
	};

public:
	CSelectServerChannel ( COuterInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CSelectServerChannel ();

public:
	void	ResetAll ();

public:
	void	CreateSubControl ();

public:
	void	AddChannel ( const CString& strChannelName, const DWORD dwColor,
		const CString& strState, const DWORD dwStateColor, const DWORD dwChannel );

	int GetSelected ()							{ return m_nIndex; }
	DWORD GetSelectedChannel ( int nIndex );

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicTextBoxEx*	m_pServerList;
	CBasicTextBox*		m_pServerState;

private:
	int		m_nIndex;
};