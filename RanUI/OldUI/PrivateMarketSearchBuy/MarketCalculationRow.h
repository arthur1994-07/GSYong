#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"

class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class GLGaeaClient;
class CBasicScrollBarEx;

class MarketCalculationRow : public CUIWindowEx
{
private:
	enum
	{
		ID_ITEM_IMAGE = ET_CONTROL_NEXT + 1,
		ID_ITEM_NAME,
		ID_ITEM_COUNT,
		ID_ITEM_COST,
	};

public:
	MarketCalculationRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketCalculationRow();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	void                LoadItemInfo();

private:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;

	CBasicTextBox*      m_pITEM_NAME;
	CBasicTextBox*      m_pITEM_COUNT;
	CBasicTextBox*      m_pITEM_COST;
	CBasicScrollBarEx*	m_pScrollBar;

public:
	void SetItemName( std::string strName, DWORD dwColor );
	void SetItemCount( WORD wTurnNum );
	void SetItemPrice( LONGLONG lnPrice, private_market::eSellType eSellType );
	void ClearMarketCalculation();
};
