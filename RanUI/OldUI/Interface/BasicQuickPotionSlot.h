#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

class	CBasicTextBox;
class	CItemImage;
class GLGaeaClient;

class CBasicQuickPotionSlot : public CUIGroup
{
	static const D3DCOLOR dwDEFAULT_TRANSPARENCY;
	static const D3DCOLOR dwFULL_TRANSPARENCY;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:
	enum
	{
		QUICK_POTION_MOUSE_OVER = NO_ID + 1,
	};

	int m_nIndex;

	CUIControl*		m_pPotionMouseOver;
	CString			m_strMiniText;
	CBasicTextBox*	m_pMiniText;
	CItemImage*		m_pItemImage;
	SNATIVEID		m_sNativeID;
    GLGaeaClient* m_pGaeaClient;

public:
	CBasicQuickPotionSlot(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicQuickPotionSlot ();

public:
	void	CreateSubControl ();

protected:		
	void	CreatePotionImage ( char* szImage );
	void	CreateMouseOver ( char* szMouseOver );

protected:
	CBasicTextBox*	CreateTextBox ( char* szTextBox );

public:
	void	CreateText ( CD3DFontPar* pFont8, CString strMiniText );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void	SetSlotIndex ( int nIndex )				{ m_nIndex = nIndex; }
	int		GetSlotIndex ()							{ return m_nIndex; }

protected:
	void	SetItem ( SNATIVEID sItemID, int nAmount );
	void	ResetItem ();

	void SetItemNativeID ( const SNATIVEID& sNativeID )		{ m_sNativeID = sNativeID; }
	const SNATIVEID& GetItemID () const						{ return m_sNativeID; }
};