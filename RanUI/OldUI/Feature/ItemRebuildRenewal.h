#pragma once

#pragma	once

#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Util/NpcActionable.h"
#include "../Feature/CostumColorInfo.h"


class CInnerInterface;
class CItemImage;
class CBasicTextButton;
class CCostumColorInfo;
class ItemInfoRebuildToolTip;
class CUIControl;
class GLGaeaClient;

//--------------------------------------------------------------------
// ItemRebuild
//--------------------------------------------------------------------
class ItemRebuildRenewal
	: public  CUIWindowEx	// ITEMREBUILD_MARK
	, private CUIControlNpcActionable 
	, public IItemRebuildRenewal
{
protected:
	enum
	{
		ITEM_REBUILD_ITEM_SLOT = ET_CONTROL_NEXT,
		ITEM_STAMP_ITEM_SLOT,
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
	CItemImage*			m_pStampImage;
	CCostumColorInfo*	m_pColorInfo;
	CBasicTextBox*		m_pCostTextBoxStatic;
	CBasicTextBox*		m_pCostTextBox;
	CBasicTextBox*		m_pMoneyTextBox;
	CBasicTextBox*		m_pGuideTextBox;
	CBasicTextButton*	m_pOkButton;
	ItemInfoRebuildToolTip*		m_pItemOption;

private:
//	RandomOpt::FixOption m_fixOption;
	SINVEN_POS m_sPreIventoryItem;
	SINVEN_POS m_sPreStampItem;

public:
	ItemRebuildRenewal( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~ItemRebuildRenewal();

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual const UIRECT& GetGlobalPos(void) override;
	virtual void SetGlobalPos(const D3DXVECTOR2& vPosition) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;
	virtual void SetNpc(const SNATIVEID& sNPCID, const DWORD dwNPCGlobalID) override;
	virtual void SetPreInventoryItem( WORD wPosX, WORD wPosY ) override;	
	virtual void GetPreInventoryItemPos( WORD& wOutPosX, WORD& wOutPosY ) override;	
	virtual void ResetPreInventoryItem(void) override;
	virtual void SetPreStampItem( WORD wPosX, WORD wPosY ) override;
	virtual void GetPreStampItemPos( WORD& wOutPosX, WORD& wOutPosY ) override;
	virtual void ResetPreStampItem(void) override;
	virtual void SetItemRebuildResetCheckBox(void) override;
	virtual void SetItemRebuildSetCheckBox(const SITEMCUSTOM& sItemCustom, const RandomOpt::FixOption& fixOption) override;


public:
	virtual	VOID Update( INT x, INT y, BYTE LB, BYTE MB, BYTE RB, INT nScroll, FLOAT fElapsedTime, BOOL bFirstControl );
	virtual	VOID TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	SNATIVEID    GetNPCID()       { return CUIControlNpcActionable::GetNPCID(); }
	DWORD        GetNPCGlobalID() { return CUIControlNpcActionable::GetNPCGlobalID(); }		

protected:
	GLGaeaClient* m_pGaeaClient;	
};