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

class MarketHistoryhRow;


class MarketHistoryPage: public CUIWindowEx
{
private:
	enum
	{

	};

public:
	MarketHistoryPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketHistoryPage();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	//virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
	virtual void        Refresh();
	void                LoadItemInfo();

	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

private:
	GLGaeaClient*       m_pGaeaClient;
	CInnerInterface*    m_pInterface;
	CBasicLineBoxSmart* m_pTitleLineBox;
	CBasicLineBoxSmart*	m_pMiddleLineBox;
	CBasicTextBox*      m_pTitleBox;

	std::vector<MarketHistoryhRow*>	m_pHistoryRow;
	Pagenavigation*		m_pPagenavigation;
	std::vector<private_market::sSaleLog*> m_sLogData;
};
