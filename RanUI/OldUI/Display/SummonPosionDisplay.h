#pragma once

#include "../Util/UIWindowEx.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class CInnerInterface;
class CItemImage;
class CBasicTextBox;
class GLGaeaClient;

class CSummonPosionDisplay : public CUIWindowEx
{

	static const D3DCOLOR dwDEFAULT_TRANSPARENCY;
	static const D3DCOLOR dwFULL_TRANSPARENCY;

	enum
	{
		SUMMON_POSION_IMAGE = ET_CONTROL_NEXT,
	};
public:
	CSummonPosionDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CSummonPosionDisplay(void);

public:
	void		   CreateSubControl ();
	CBasicTextBox* CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CItemImage*	   CreateItemImage ( const char* szControl, UIGUID ControlID );
public:
	virtual void   Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void   TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void   SetVisibleSingle ( BOOL bVisible );
protected:
	void	SetItem ( SNATIVEID sICONINDEX, const char* szTexture );	
	void	ResetItem ();

private:
	GLGaeaClient*	m_pGaeaClient;
	CItemImage*		m_pPosionImage;
	CBasicTextBox*  m_pAmountText;    
};
