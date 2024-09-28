#pragma	once

#include "UIGroup.h"

class	CBasicButton;
class	CUIWindowBody;
class	CUIWindowTitle;

class CUIWindow : public CUIGroup
{
protected:
	enum	//	�⺻ ������ ID ( �ܺη� �������� ���� )
	{
		ET_CONTROL_TITLE = NO_ID + 1,
		ET_CONTROL_TITLE_F,
		ET_CONTROL_BUTTON,
		ET_CONTROL_BODY,
		ET_CONTROL_MESSAGEBOX,		// ���� �ڵ���� ȣȯ�� ���� ������� �ø���;
		ET_CONTROL_NEXT,
	};

public:
	CUIWindow (EngineDeviceMan* pEngineDevice);
	virtual	~CUIWindow ();

public:
	void CreateTitle( char* szFrame, char* szLeft, char* szMid, char* szRight, char* szTextBox, const char* szText );
	void CreateTitleFocus( char* szFrame, char* szLeft, char* szMid, char* szRight, char* szTextBox, const char* szText );
	void CreateCloseButton( char* szValue, char* szFlipValue, char* szOver );
	void CreateBody( char* szFrame, char* szLeft, char* szUp, char* szMain, char* szDown, char* szRight );

public:
	void SetTitleName( const char* szTitleName );
	void SetTitleAlign( int nAlign );
	void SetFocusWindow( BOOL bFocus );

public:
	virtual void SetFocusControl();
	virtual void ResetFocusControl();

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void ResetExclusiveControl();

public:
	virtual CUIWindowBody* GetBody()	{ return m_pBody; }

protected:	
	CBasicButton*	m_pClose;
	CUIWindowBody*	m_pBody;
	CUIWindowTitle*	m_pTitle;
	CUIWindowTitle*	m_pTitleFocus;
	
protected:
	BOOL			m_bFirstGap;
	D3DXVECTOR2		m_vGap;

	int				m_PosX;
	int				m_PosY;
};

#define INIT_ARRAY_NULL( VARIABLE, MAX ) for( int i = 0; i < MAX; ++i ) VARIABLE[i] = NULL;
#define DCAST( TYPE, VARIABLE ) dynamic_cast< TYPE* >( VARIABLE )
#define IDCTRL( INDEX ) INDEX + ET_CONTROL_NEXT + 1