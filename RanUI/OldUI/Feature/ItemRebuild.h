#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"
#include "../Feature/CostumColorInfo.h"

class CInnerInterface;
class CItemImage;
class CBasicTextButton;
class CCostumColorInfo;
class GLGaeaClient;

//--------------------------------------------------------------------
// ItemRebuild
//--------------------------------------------------------------------
class CItemRebuild 
    : public  CUIWindowEx	// ITEMREBUILD_MARK
    , private CUIControlNpcActionable 
{
protected:
	enum
	{
		ITEM_REBUILD_ITEM_SLOT = ET_CONTROL_NEXT,
		ITEM_REBUILD_COAST_TEXT_STATIC,
		ITEM_REBUILD_COAST_TEXT,
		ITEM_REBUILD_MONEY_BACK,
		ITEM_REBUILD_MONEY_IMAGE,
		ITEM_REBUILD_MONEY_TEXT,
		ITEM_REBUILD_OK_BUTTON,
		ITEM_REBUILD_CANCEL_BUTTON,
		ITEM_COLOR_INFO,
	};

protected:
	CItemImage*			m_pItemImage;
	CCostumColorInfo*	m_pColorInfo;
	CBasicTextBox*		m_pCostTextBoxStatic;
	CBasicTextBox*		m_pCostTextBox;
	CBasicTextBox*		m_pMoneyTextBox;
	CBasicTextButton*	m_pOkButton;

private:
    SINVEN_POS m_sPreIventoryItem;

public:
	CItemRebuild( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CItemRebuild();

public:
	VOID CreateSubControl();


public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void         SetNpc( const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID );

public:
    SNATIVEID    GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
    DWORD        GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }

    void SetPreInventoryItem( WORD wPosX, WORD wPosY );
    void GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY );
    void ResetPreInventoryItem();

protected:
	GLGaeaClient* m_pGaeaClient;

};