#pragma once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"
#include "MarketSearchLuaLoad.h"

class PrivateMarketClient;
struct SSEARCHPAGEDATA;

class CBasicComboBoxRollOver;
class CSmartComboBoxBasicEx;
class CBasicLineBoxSmart;
class CBasicTextBoxEx;
class CBasicTextBox;
class CMessageBox;
class GLGaeaClient;
class CSwapImage;
class CCheckBox;
class CUIEditBoxMan;
class Pagenavigation;
class RnButton;

class MarketSearchRow;

const int MAX_ROLLOVER_TEXT = 10;
const int NULL_PAGE = -1;
const int NULL_ROW = -1;

class MarketSearchPage : public CUIWindowEx
{
	typedef std::list<CString>	SRCLIST;
	typedef SRCLIST::iterator	SRCLIST_ITER;

private:
	enum
	{
		ID_NULL = ET_CONTROL_NEXT + 1,

		ID_ITEM_SEARCH_EDITBOX_1_BG,

		ID_ITEM_SEARCH_EDITBOX,
		ID_ITEM_SEARCH_EDITBOX_1,
		ID_ITEM_SEARCH_COMBOX,

		ID_BASIC_STATS,
		ID_BASIC_STATS_1,

		ID_BASIC_ITEM_TURN,
		ID_BASIC_ITEM_TURN_1,

		ID_ITEM_NAME_SORT,
		ID_ITEM_COST_SORT,

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

		ID_NOMAL_STATS_EDITBOX,
		ID_NOMAL_STATS_EDITBOX_1,
		ID_NOMAL_STATS_EDITBOX_2,
		ID_NOMAL_STATS_EDITBOX_3,
		ID_NOMAL_STATS_EDITBOX_4,

		ID_ADD_RANDOM_EDITBOX, //가산및 램덤 옵션
		ID_ADD_RANDOM_EDITBOX_1,
		ID_ADD_RANDOM_EDITBOX_2,
		ID_ADD_RANDOM_EDITBOX_3,
		ID_ADD_RANDOM_EDITBOX_4,

		ID_SEARCH_OPTION_RESET_BUTTON,
		ID_ITEM_SEARCH_BUTTON,
		ID_ADD_WHISH_BUTTON,
		ID_BUY_BUTTON,
		ID_PAGE_NAVIGATION,

        ID_BUY_MESSAGEBOX,  //구매확인 메시지박스

		ID_ITEM_CATEGORY_COMBO,
		ID_BASIC_WEAR_CATEGORY_COMBO,
		ID_BASIC_STATS_CATEGORY_COMBO,
		ID_BASIC_LEVEL_CATEGORY_COMBO,
		ID_BASIC_CLASS_CATEGORY_COMBO,
		ID_BASIC_SEX_CATEGORY_COMBO,

		ID_NOMAL_STATS_COMBO_1,
		ID_NOMAL_STATS_COMBO_2,
		ID_NOMAL_STATS_COMBO_3,
		ID_NOMAL_STATS_COMBO_4,

		ID_ADD_RANDOM_OPTION_COMBO_BG_1,
		ID_ADD_RANDOM_OPTION_COMBO_BG_2,
		ID_ADD_RANDOM_OPTION_COMBO_BG_3,
		ID_ADD_RANDOM_OPTION_COMBO_BG_4,

		ID_BLOW_COMBOX_1,
		ID_BLOW_COMBOX_2,
		ID_BLOW_COMBOX_3,
		ID_BLOW_COMBOX_4,

		ID_SEARCH_SELL_TYPE_GOLD_CHECKBOX,
		ID_SEARCH_SELL_TYPE_POINT_CHECKBOX,

		ID_COMBOX_START = ID_ITEM_CATEGORY_COMBO,
		ID_COMBOX_END = ID_BLOW_COMBOX_4,
		ID_COMBOX_COUNT = ID_COMBOX_END - ID_COMBOX_START,
	};



public:
	MarketSearchPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	~MarketSearchPage();
	void	            CreateSubControl();
	virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	//void				ComboxTranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	//virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
	//virtual void        Refresh();
	void                LoadItemInfo();

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;
	//LoadMarketLuaFile m_SearchOption;
	std::vector<private_market::sSaleItem*> m_vecDataPage;
	private_market::sSaleItemSearchandSortBasic m_sBasic;
	private_market::sSaleItemSearchandSortName m_sName;
	void SetRollOverCategory( std::vector<UI_OPTION> UiOption, CSmartComboBoxBasicEx* pComBox ); 
	void SetRollOverCategory( std::string* strComment, int nSize, CSmartComboBoxBasicEx* pComBox );
	//void LoadSearchFile();
	//void LoadSearchFileFromITEM();
	void UpdateComoboItem();
	void ComboInit();
	void SetComBoxClickEnevt( UIGUID ControlID, DWORD dwMsg );
	void GetPageData();// 현재페이지의 아이템 데이터를 가져온다.
	void SetVisibleFALSESearchRow();//검색된 아이템 열을 오프한다.
	void SetSearchRow();//m_vecDataPage의 데이터를 UI에 출력한다.
	void PagenavigationUpdate();
	void SetPageWaiting(bool bIsWaiting);
	bool IsPageWaiting();
	bool m_bPageWaiting;
	bool IsItemName();

	void SetSearchSellTypeCheckBox( int nCheckBoxID );




private:
	GLGaeaClient*           m_pGaeaClient;
	CInnerInterface*        m_pInterface;
	CBasicLineBoxSmart*     m_pTitleLineBox;
	CBasicLineBoxSmart*	    m_pMiddleLineBox;
	CBasicTextBox*          m_pTitleBox;
	CBasicTextBox*		    m_pMoneyTextBox;
    CBasicTextBox*		    m_pRanPointTextBox;
	CBasicTextBox*			m_pWaitingBox;
    CBasicTextBox*			m_pNoExistResultBox;

	//아이템 리스트 자동완성
	CBasicComboBoxRollOver*	m_pComboRollSearchOver;
	std::vector<CString>	m_vecString;
	//SRCLIST					m_listSrcString;
	int						m_nSelectString;
	int						m_nPageNum;
	int						m_nBeforeStrLength;
	int						m_nSortName;
	int						m_nSortPrice;

	//검색결과
	std::vector<MarketSearchRow*>	m_pSearchRow;
	Pagenavigation*		m_pPagenavigation;
	CBasicLineBoxSmart* m_pSelectLine;
public:
	void RefreshSearchResult();
	void ReSearchItem(private_market::eItemSortOption emSortOption);

	RnButton*               m_pSearchButton;    //검색 버튼
    CMessageBox*		    m_pBuyMessageBox;  //구매확인 메시지박스

    //검색조건
    //CUIEditBoxLink*		 m_pItemSearchEditBox;
	CCheckBox*              m_pCheckBoxGold;
	CCheckBox*              m_pCheckBoxPoint;

    std::string				m_strArrowAsc;
	std::string				m_strArrowDesc;
    CSwapImage*             m_pNameSortImage;
	CSwapImage*             m_pPriceSortImage;
	CCheckBox*              m_pLimiteItemCheckBox;
	CBasicTextBox*          m_pBasicWearTexBox;
	CSwapImage*             m_pClassImage;
	CUIEditBoxMan*          m_pItemSearchEditBox;
	CUIEditBoxMan*          m_pAddRandomEditBox;
	CUIEditBoxMan*          m_pBasicStatsEditBox;
	CSmartComboBoxBasicEx*  m_pComboBox_Design;
	CBasicComboBoxRollOver* m_pComboBox_RollOver_Design;
    std::vector<CSmartComboBoxBasicEx*> m_pSearchOptionCombox; 

	void ResetSearchOption();
	void ResetSearchItemName();
	void ResetBasicOption();//기본조건
	void ResetNomalOPtion();//일반능력치
	void ResetRandoOption();//가산및 랜덤 옵션
	void ResetBLOWCombox();//상태이상 콤보박스
	void CloseAllRollOver();
	void SetItemCategoryCombo(int nIndex);


	int m_nSelectPage; //선택 페이지
	int m_nSelectRow;  // 선택 열

	// 판매 아이템 정보 세팅 //
	void SetsSaleItem( int nIndex, private_market::sSaleItem* sSaleItem);
	void SetUUID( int nIndex, UUID uuidID );
	void SetItemImage( int nIndex, const SITEMCUSTOM& sItemCustom, WORD wTurnNum );
	void SetMapName( int nIndex, const CString& strtext, DWORD dwColor);
	void IsUseItem( int nIndex, const SITEMCUSTOM& sItemCustom );
	void SetItemName( int nIndex, const SITEMCUSTOM& sItemCustom );
	void SetTitemSellerName( int nIndex, const CString& strText );
    //void SetSellType( int nIndex, private_market::eSellType eSellType );
	void SetTitemSellerPos();
	void SetItemPrice( int nIndex, LONGLONG lnPrice, private_market::eSellType eSellType  );
	void SetItemCount();
	//--------------------------//
	void SetClassSexTexture();
	void SetVisibleSingleNomalStats( BOOL bBool );
	void SetVisibleSingleAddRandomOption( BOOL bBool );

	CBasicLineBoxSmart* m_pLineBoxSearchNomalStatsBG;	//검색 페이지 일반능력치 배경
	CBasicTextBox* m_pBasicNomalStatsTextBox;	// [일반 능력치] 텍스트 박스
	CUIEditBoxMan* m_pItemStatsEditBox;	// [일반능력치 에디트 박스]
	CUIEditBoxMan* m_pBasicItemTurnNumEditBox;//아이템 용량

	CString GetItemNameSearch();
	int GetItemType();
	BOOL GetTimeLimit();
	DWORD GetItemTurnNum();
	int GetWeaponType();
	int GetArmorType();
	int GetItemStatsType();// 힘, 민첩, 정신
	int GetItemStatsValue();
	int GetItemLevelCategory();
	int GetClassCategory();
	int GetSexCategory();
	//-- 일반능력치
	int GetNomalStatsCategoryType_1();
	int GetNomalStatsCategoryType_2();
	int GetNomalStatsCategoryType_3();
	int GetNomalStatsCategoryType_4();
	float GetNomalStatsCategoryValue_1();
	float GetNomalStatsCategoryValue_2();
	float GetNomalStatsCategoryValue_3();
	float GetNomalStatsCategoryValue_4();
	//-- 가산 및 랜덤 옵션
	int GetAddRandomOptionCategoryType_1();
	int GetAddRandomOptionCategoryType_2();
	int GetAddRandomOptionCategoryType_3();
	int GetAddRandomOptionCategoryType_4();
	float GetAddRandomOptionCategoryValue_1();
	float GetAddRandomOptionCategoryValue_2();
	float GetAddRandomOptionCategoryValue_3();
	float GetAddRandomOptionCategoryValue_4();

	int GetSearchSellType();
};
