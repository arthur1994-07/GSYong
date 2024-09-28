#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"
#include "../../../RanLogic/Item/GLItem.h"

class CInnerInterface;
class CItemImage;
class CBasicTextButton;
class CBasicAnimationBox;
class GLGaeaClient;

//--------------------------------------------------------------------
// CItemMixWindow : 
//--------------------------------------------------------------------
class CItemMixWindow 
    : public  CUIWindowEx
    , private CUIControlNpcActionable
{
protected:
	enum
	{
		ITEM_MIX_SUB_SLOT0 = ET_CONTROL_NEXT,
		ITEM_MIX_SUB_SLOT1,
		ITEM_MIX_SUB_SLOT2,
		ITEM_MIX_SUB_SLOT3,
		ITEM_MIX_SUB_SLOT4,
		ITEM_MIX_OK_BUTTON,
		ITEM_MIX_CANCEL_BUTTON,
		ITEM_MIX_IMAGE,
	};

protected:
	CItemImage*			m_pMeterialItemImage[ITEMMIX_ITEMNUM];
	CBasicTextBox*		m_pMeterialItemNum[ITEMMIX_ITEMNUM];
	CUIControl*			m_pMeterialBackImage[ITEMMIX_ITEMNUM];
	
	CBasicAnimationBox*	m_pAniBox;
	CBasicTextButton*	m_pOkButton;
	CUIControl*			m_pBackImage;

	CUIControl*			m_pSuccessImage;
	CUIControl*			m_pFailImage;

	CBasicTextBox*		m_pMsgBox;

	bool	m_bOK;
	bool	m_bResult;
	float	m_fTime;

	CString	m_strResultMsg;
	CString m_strBasicMsg;

public:
	CItemMixWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CItemMixWindow();

public:
	VOID CreateSubControl();
	VOID DoItemMix();
	void Reset();
	void SetAnimation();
	void SetItemMixResult( CString strMsg, bool bSuccess = false, bool bFail = false );

public:
    void SetNpc( const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID );

public:
    SNATIVEID    GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD        GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

protected:
	CBasicTextBox*	CreateTextBox ( const char* szTextBox );

public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );

protected:
	GLGaeaClient* m_pGaeaClient;

};