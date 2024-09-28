#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BASICINFOVIEW_LBDUP = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class	CInnerInterface;
class	CBasicProgressBar;
class	CBasicTextBox;
class	GLGaeaClient;

class CBasicInfoView : public CUIGroup
{
private:
	enum
	{
		HP_TEXT = NO_ID + 1,
		MP_TEXT,
		SP_TEXT,
		EXP_TEXT,
		CP_TEXT
	};

public:
	CBasicInfoView(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicInfoView();

public:
	void	CreateSubControl();

public:
	void	CheckMouseState();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
	void	SetHP( const int nNOW, const int nMAX );
	void	SetMP( const int nNOW, const int nMAX );
	void	SetSP( const int nNOW, const int nMAX );
	void	SetEXP( LONGLONG lNOW, LONGLONG lMAX, WORD wLevel );
	void	SetCP( WORD wNOW, WORD wMAX );

private:
	CBasicTextBox*		m_pHPText;
	CBasicTextBox*		m_pMPText;
	CBasicTextBox*		m_pSPText;
	CBasicTextBox*		m_pEXPText;
	CBasicTextBox*		m_pCPText;

private:
	CBasicProgressBar*	m_pHP;
	CBasicProgressBar*	m_pMP;
	CBasicProgressBar*	m_pSP;
	CBasicProgressBar*	m_pEXP;
	CBasicProgressBar*	m_pCP;

private:
	int		m_nHPBACK;
	int		m_nMPBACK;
	int		m_nSPBACK;
	LONGLONG	m_lEXPBACK;
	WORD	m_wCPBACK;
	WORD	m_wLevelBACK;

private:
	BOOL		m_bFirstGap;
	D3DXVECTOR2	m_vGap;

	int			m_PosX;
	int			m_PosY;

	bool		m_bCHECK_MOUSE_STATE;

protected:
	CInnerInterface* m_pInterface;
	GLGaeaClient*	 m_pGaeaClient;

};

class MyBasicInfoView : public IBasicInfoView, private CBasicInfoView
{
public:
    MyBasicInfoView( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyBasicInfoView()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual const UIRECT& GetUIWindowGlobalPos();
    virtual void SetUIWindowGlobalPos( const D3DXVECTOR2& vPos );
};