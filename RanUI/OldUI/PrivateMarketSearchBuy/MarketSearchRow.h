#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"

#include "../../../RanLogic/Market/MarketDefine.h"

class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class GLGaeaClient;
class CCheckBox;
class CItemImageEx;
class CUIEditBoxMan;

class MarketSearchRow : public CUIWindowEx
{
private:
	enum
	{
		ID_ITEM_IMAGE = NO_ID + 1,
		ID_ROW,
		ID_ROW_SELECT,
		ID_CHECK_BOX,
		ID_SELLER_NAME,
		ID_SELLER_POS,
		ID_ITEM_COST,
		ID_ITEM_COUNT,
		ID_COUNT_EDITBOX,
		ID_COUNT_EDITBOX_1,
	};

public:
	MarketSearchRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketSearchRow();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void                LoadItemInfo();
	void				SetCheckBoxVisible( BOOL bBool );	

private:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	CItemImageEx*		m_pITEM_IMAGE;
	CBasicTextBox*      m_pITEM_NAME;
	CBasicTextBox*      m_pISUSE;
	CBasicTextBox*      m_pSELLER_NAME;
	CBasicTextBox*      m_pSELLER_POS;
	CBasicTextBox*      m_pITEM_COST;
	CBasicTextBox*      m_pITEM_COUNT_BUY;
	CCheckBox*			m_pCheckBox;
	CBasicLineBoxSmart* m_pRowBGLine;
	CBasicLineBoxSmart* m_pSelectLine;
	CUIEditBoxMan*		m_pItemCountEditBox;
	CBasicLineBoxSmart* m_pItemCountEditBoxBG;
	CUIControl*			m_pSelectRow;// 노란색 사각형 네모 라인

    CUIControl* m_pRanPointIcon;
	CUIControl* m_pGoldIcon;

	UUID m_sItemGUID;
	SITEMCUSTOM			m_sItemCustom;
	LONGLONG m_llPrice;
	private_market::sSaleItem* m_psSaleItem;
public:
	// 판매 아이템 정보 세팅 //
	void SetsSaleItem(private_market::sSaleItem* sSaleItem);
	private_market::sSaleItem* GetsSaleItem();
	void SetUUID(UUID uuidID);
	UUID GetUUID();
    void SetMapName( const CString strMapName, DWORD dwColor = NS_UITEXTCOLOR::DEFAULT );
	LONGLONG GetPrice();
	int GetItemCount();
	void SetItemImage( const SITEMCUSTOM& sItemCustom, WORD wTurnNum = 1 );
	void IsUseItem( const SITEMCUSTOM& sItemCustom );
	void SetItemName( const SITEMCUSTOM& sItemCustom );
	void SetTitemSellerName( const CString& strText );
    //void SetSellType( private_market::eSellType eSellType );
	void SetTitemSellerPos();
	void SetItemPrice( LONGLONG lnPrice, private_market::eSellType eSellType  );
	void SetItemCount();
	BOOL IsChecked();
	void SetCheckOff();
	void SetCheckOn();
	void ItemCoutntEditBoxClear();
	//--------------------------//

	SITEMCUSTOM GetItemData();
};
