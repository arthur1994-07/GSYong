#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"

class CBasicComboBoxRollOver;
class CSmartComboBoxBasicEx;
class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class GLGaeaClient;
class CSwapImage;
class CCheckBox;
class CUIEditBoxMan;
class Pagenavigation;

class MarketSearchRow;
class MarketCalculationRow;
class CMessageBox;


class MarketSearchWhishPage : public CUIWindowEx
{
private:
	enum
	{
		ID_NULL = ET_CONTROL_NEXT + 1,

		ID_MESSAGEBOX,
		ID_DEL_CHECK_ROW,
		ID_ALL_CHECKBOX,
		ID_PAGE_NAVIGATION,
		ID_ITEM_COMPARE_BUTTON,
		ID_CHECK_BUY,
		ID_ROW1,
		ID_ROW2,
		ID_ROW3,
		ID_ROW4,
		ID_ROW5,
		ID_ROW6,
		ID_ROW7,
		ID_ROW8,
		ID_ROW_START = ID_ROW1,
		ID_ROW_END = ID_ROW8,

	};



public:
	MarketSearchWhishPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketSearchWhishPage();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	//virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
	//virtual void        Refresh();

	void                LoadItemInfo();

	UIGUID	            m_RollOverID;
	BOOL	            m_bFirstLBUP;
private:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;
	CBasicLineBoxSmart* m_pTitleLineBox;
	CBasicLineBoxSmart*	m_pMiddleLineBox;
	CBasicTextBox*      m_pTitleBox;
	CCheckBox*			m_pAllCheckBox;

	std::vector<MarketSearchRow*>	m_pSearchRow;
	Pagenavigation*		m_pPagenavigation;

	CMessageBox*		m_pMessageBox;

	// 계산대
	MarketCalculationRow* m_pMarketCalculation;
	LONGLONG m_nCalculationItemCost;
	CBasicTextBoxEx*	m_pItemCostTextBox;
	CBasicTextBoxEx*	m_pHaveMoneyTextBox;
	CBasicTextBoxEx*	m_pAfter_MoneyTextBox;

    LONGLONG m_nCalculationItemCostRP;
    CBasicTextBoxEx*	m_pItemCostTextBoxRP;
    CBasicTextBoxEx*	m_pHaveMoneyTextBoxRP;
    CBasicTextBoxEx*	m_pAfter_MoneyTextBoxRP;
	std::vector<private_market::sSaleItem*> m_vecDataPage;
public:
	// 판매 아이템 정보 세팅 //
	void SetsSaleItem( int nIndex, private_market::sSaleItem* sSaleItem);
	void SetUUID( int nIndex, UUID uuidID );
	void SetMapName( int nindex, const CString strText);
	void SetItemImage( int nIndex, const SITEMCUSTOM& sItemCustom, WORD wTurnNum = 1 );
	void IsUseItem( int nIndex, const SITEMCUSTOM& sItemCustom );
	void SetItemName( int nIndex, const SITEMCUSTOM& sItemCustom );
	void SetTitemSellerName( int nIndex, const CString& strText );
	void SetTitemSellerPos();
	void SetItemPrice( int nIndex, LONGLONG lnPrice, private_market::eSellType eSellType );
	void SetItemCount();
	//--------------------------//
	virtual void Refresh();
	void DelCheckRow();

};
