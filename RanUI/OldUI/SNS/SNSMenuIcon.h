
// bjju.sns

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"
#include "../EngineLib/GUInterface/UITextControl.h"

#include "../EngineLib/G-Logic/GLDefine.h"

class CSwapImage;
class CBasicTextBox;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_SNS_MENU_ICON_PUSH = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CSNSMenuIcon : public CUIGroup
{
protected:
	enum
	{
		MENU_ICON_NONE  = NO_ID,

		MENU_ICON_IMAGE,
		MENU_ICON_TEXT,
	};

public:
	CSNSMenuIcon(EngineDeviceMan* pEngineDevice);
	virtual ~CSNSMenuIcon();

public:
	static const D3DCOLOR	cCOLOR_ICON_DEFAULT_DIFFUSE	= D3DCOLOR_ARGB( 255, 220, 220, 220 );
	static const D3DCOLOR	cCOLOR_ICON_OVER_DIFFUSE	= NS_UITEXTCOLOR::WHITE;
	static const D3DCOLOR	cCOLOR_ICON_PUSH_DIFFUSE	= NS_UITEXTCOLOR::DARKGRAY;
	static const D3DCOLOR	cCOLOR_ICON_DISABLE			= D3DCOLOR_ARGB( 255, 100, 100, 100 );

public:
	virtual void CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void			SetImage  ( const char* strTextureInfo );
	void			SetText   ( const CString& strText );
	void			SetEnable ( const BOOL bEnable );

private:
	CSwapImage*		m_pImage;
	CBasicTextBox*	m_pText;
	BOOL			m_bEnable;

};