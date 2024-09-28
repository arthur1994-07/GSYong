#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicLineBox;
class CBasicTextBox;
class CBasicTextButton;
class CBasicComboBox;
class CBasicComboBoxRollOver;
class CUIEditBoxMan;
class GLGaeaClient;

class CShopItemSearchWindow : public CUIWindowEx
{
	typedef std::list<CString>	SRCLIST;
	typedef SRCLIST::iterator	SRCLIST_ITER;

protected:
	enum
	{
		ITEMSHOPSEARCH_BUTTON1 = ET_CONTROL_NEXT,
		ITEMSHOPSEARCH_BUTTON2,
		ITEMSHOPSEARCH_MAN,
		ITEMSHOPSEARCH_EDIT,
		ITEMSHOPSEARCH_COMBO_ROLLOVER,
		ITEMSHOPSEARCH_COMBO_OPEN1,
		ITEMSHOPSEARCH_COMBO_OPEN2,
		ITEMSHOPSEARCH_COMBO_OPEN3,
		ITEMSHOPSEARCH_COMBO_OPEN4,
		ITEMSHOPSEARCH_COMBO_ROLLOVER1,
		ITEMSHOPSEARCH_COMBO_ROLLOVER2,
		ITEMSHOPSEARCH_COMBO_ROLLOVER3,
		ITEMSHOPSEARCH_COMBO_ROLLOVER4
	};

public:
	CShopItemSearchWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~CShopItemSearchWindow(void);

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl();

	CBasicTextButton* CreateTextButton( char* szButton, UIGUID ControlID, char* szText );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );


private:
	void	ResetAll();
	void	UpdateComoboItem();
	void	ComboInit();
	void	LoadSearchFile();

	void	ReqSearch();

public:
	virtual	void SetVisibleSingle( BOOL bVisible );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	SSEARCHITEMDATA GetSearchData() { return m_sSearchData; }

private:	
	CUIEditBoxMan*			m_pItemNameEdit;
	CBasicComboBox*			m_pComboBoxOpen[4];
	CBasicComboBoxRollOver*	m_pComboRollOver[4];
	CBasicComboBoxRollOver*	m_pComboRollSearchOver;

	UIGUID					m_RollOverID;
	BOOL					m_bFirstLBUP;

	std::vector<CString>	m_vecString;
	SRCLIST					m_listSrcString;
	int						m_nSelectString;
	int						m_nPageNum;
	int						m_nBeforeStrLength;

	SSEARCHITEMDATA			m_sSearchData;

	CTime					m_searchTime;
	BOOL					m_bSearch;

};

class MyShopItemSearchWindow : public IShopItemSearchWindow, private CShopItemSearchWindow
{
public:
    MyShopItemSearchWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyShopItemSearchWindow()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual SSEARCHITEMDATA GetSearchData();
};
