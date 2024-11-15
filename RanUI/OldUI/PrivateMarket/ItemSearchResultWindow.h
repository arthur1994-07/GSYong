#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicLineBox;
class CBasicTextBox;
class CBasicTextBoxEx;
class CBasicTextButton;
class CUIEditBoxMan;
class CItemSlot;
class GLGaeaClient;

class CItemSearchResultWindow : public CUIWindowEx
{
private:
	enum
	{
		ITEM_SEARCH_RESULT_BUTTON1 = ET_CONTROL_NEXT,
		ITEM_SEARCH_RESULT_BUTTON2,
		ITEM_SEARCH_RESULT_BUTTON3,
		ITEM_SEARCH_RESULT_BUTTON4,
		ITEM_SEARCH_RESULT_BUTTON5,
		ITEM_SEARCH_RESULT_LIST1,
		ITEM_SEARCH_RESULT_LIST2,
		ITEM_SEARCH_RESULT_LIST3,
		ITEM_SEARCH_RESULT_LIST4
	};

public:
	CItemSearchResultWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CItemSearchResultWindow(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void			  CreateSubControl();
	CBasicTextButton* CreateTextButton( char* szButton, UIGUID ControlID, char* szText );
	CBasicTextBox*	  CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	void			  UpdateSearchResult(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum);
	
public:
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

private:
	CBasicTextBoxEx* m_pItemNameList;
	CBasicTextBoxEx* m_pShopNameList;
	CBasicTextBoxEx* m_pPosList;
	CBasicTextBoxEx* m_pPriceList;
	CBasicTextBox*	 m_pPageStatic;	

	std::vector<SSEARCHITEMESULT> m_vecSarchResult;

	bool			 m_bNextPage;
	bool			 m_bPrePage;
	DWORD			 m_dwCurrentPage;

	CTime			 m_reSearchTime;

};

class MyItemSearchResultWindow : public IItemSearchResultWindow, private CItemSearchResultWindow
{
public:
    MyItemSearchResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyItemSearchResultWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void UpdateSearchResult(SSEARCHITEMESULT* pSearchItemResult, DWORD dwUpdateNum, DWORD dwPageNum);
};