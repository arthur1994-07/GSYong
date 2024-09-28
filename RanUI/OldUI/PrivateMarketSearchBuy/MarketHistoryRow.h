#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"

class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class GLGaeaClient;
class CCheckBox;
class Pagenavigation;


class MarketHistoryhRow : public CUIWindowEx
{
private:
	enum
	{
		ID_ITEM_IMAGE = ET_CONTROL_NEXT + 1,
		ID_ITEM_NAME,
		ID_ISUSE,
		ID_SELLER_NAME,
		ID_SELLER_POS,
		ID_ITEM_COST,
		ID_ITEM_COUNT,
	};

public:
	MarketHistoryhRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketHistoryhRow();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void                LoadItemInfo();
	void				SetCheckBoxVisible( BOOL bBool );

private:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	CBasicTextBox*      m_pDatePurchaseTextBox;
	CBasicTextBox*      m_pTimeTextBox;
	CBasicTextBox*      m_pTransactionTextBox;
	CBasicTextBox*      m_pItemNameBox;
	CBasicTextBox*      m_pItemCountTextBox;
	CBasicTextBox*      m_pItemPriceTextBox;
    CUIControl*         m_pRanPointIcon;
	CBasicTextBox*		m_pSellerNameTextBox;

public:
	void SetDatePurchaseText( const CString strText );
	void SetTimeTextBox( const CString strText );
	void SetTransactionText( const CString strText, DWORD dwColor );
	void SetItiemNamText( const SNATIVEID sItemID );
	void SetItemCountText( const DWORD dwCount );
    void SetItemPriceText( const LONGLONG llPrice, int nSellType, const bool bIsCommission = false, const float fCommissionRate = 0 );
	void SetSellerNameText( const CString strText );    

};
