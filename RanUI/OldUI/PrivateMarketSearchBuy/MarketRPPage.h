#pragma once

#include "../Util/UIGroupHelperMessageBox.h"

#include "../../../RanLogic/Market/GLConsignmentSaleDefine.h"
#include "../../../RanLogic/Item/GLItem.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class Pagenavigation;
class CUIEditBox;
class CItemImageEx;
class CSmartComboBoxBasicEx;
class RnButton;
class CCheckBox;

struct SNATIVEID;

class MarketRPPage : public CUIGroupHelperMessageBox
{
    class MarketRPPageRow : public CUIGroupHelper
    {
        enum
        {
            ID_NOT_USE = NO_ID, 
            ID_ITEM_IMAGE,
            ID_ROW,
            ID_CHECK_BOX,
            ID_COUNT_EDITBOX,
            ID_COUNT_EDITBOX_1,

        };
    public:
        MarketRPPageRow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );;
        void	            CreateSubControl();

        virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );;
        virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

        void                SetItem( const private_market::ConsignmentSaleItem& SaleItem );
        private_market::ConsignmentSaleItem GetItem();

        int                 GetRemainTime();
        bool                IsMouseOnImageICON(){ return m_bMouseOnImage; };

    private:
        GLGaeaClient*       m_pGaeaClient;
        CInnerInterface*    m_pInterface;
        
        private_market::ConsignmentSaleItem m_SaleItem;
        bool                m_bMouseOnImage;

    private:
        CItemImageEx*       m_pItemImage;
        CBasicTextBox*      m_pITEM_NAME;
        CBasicTextBox*      m_pISUSE;           //착용가능여부
        CBasicTextBox*      m_pSELLER_NAME;
        CBasicTextBox*      m_pExpirationDate;  //마감시간
        CBasicTextBox*      m_pITEM_COST;       //금액
        CBasicTextBox*      m_pTurnNum;         //팔린 갯수
       
        CUIControl*         m_pRanPointIcon;
    };

    enum
    {
        emRowSize = 8,
    };
    enum
    {
        ID_NOT_USE = ID_NEXT,
        ID_BUTTON_UNREGIST,
        ID_BUTTON_RESET,
        ID_BUTTON_REGIST,

        ID_PAGENAVIGATION,

        ID_IMAGE_ITEM,
        ID_TEXTBOX_NUM_OF_SALE,
        ID_TEXTBOX_SALE_PRICE,

        ID_MONEY_COMBO,
        ID_COMBO,

        ID_CHECKBOX_RANPOINT,
        ID_CHECKBOX_RANMONEY,

        ID_ROW_START = 100,
        ID_ROW_END = ID_ROW_START + emRowSize,
    };


public:
    MarketRPPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void	            CreateSubControl();

    virtual void        Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg );
    virtual void        SetVisibleSingle( BOOL bVisible );
    
    void                AllEndEdit();
    void                ListUpdate();
    void                Reset();
    void                Regist();
    void                Unregist();

    void                Refresh();
    

private:
    // 변경되는 텍스트들
    void                SetRegistCountText( UINT nNow, UINT nMax, UINT nExtended );
    void                SetSaleCommissionText( float fRate );
    void                SetPeeText( const SNATIVEID ItemID );
    void                SetPeeGameMoneyText( unsigned int nIndex );
    // 판매되는 아이템 이미지
	void				SetImage( const SITEMCUSTOM& sItemCustom );
    void                SetImage( const SNATIVEID ItemID, WORD wTurnNum );
    // 판매되는 아이템 가격 텍스트 리셋
    void                ResetSaleCountBox();
    void                ResetSalePriceBox();

    bool                RegistCheck();


    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    Pagenavigation*     m_pPagenavigation;
    MarketRPPageRow*    m_pRow[emRowSize];
    CBasicLineBoxSmart* m_pSelectImg;
    CSmartComboBoxBasicEx* m_pPeriodGameMoney;   // 판매 게임머니
    CBasicTextBox*      m_pRegistCountText;
    CBasicTextBox*      m_pExtendedHoursText;
    RnButton*           m_pButton;
    CBasicTextBox*      m_pSaleCommissionText;  // 판매 수수료
    CBasicTextBox*      m_pRegistValue;         // 등록 비용
    CBasicTextBox*      m_pMyMoneyText;         // 보유 머니
    CBasicTextBox*      m_pMyPointText;         // 보유 포인트
    CSmartComboBoxBasicEx* m_pPeriodComboBox;   // 판매 등록시간
    CItemImageEx*       m_pItemImage;

    CUIEditBox*         m_pSaleCount;
    CUIEditBox*         m_pSalePrice;
	CBasicTextBox*      m_pSaleCountText;
	CBasicTextBox*      m_pSalePriceText;

    CCheckBox*          m_pCheckBoxPoint;
    CCheckBox*          m_pCheckBoxMoney;
	CBasicTextBox*      m_pTextBoxMoney;

private:
    private_market::SALE_ITEMS_VECTOR m_vecItemList;
    bool                m_bListUpdated;
    int                 m_nSeleted;
    int                 m_nSeletedTemp;

private:
    int                 m_nCount;
    LONGLONG            m_llPrice;

    SINVENITEM          m_sInvenItem;           // 현재 등록하려는 아이템
};
