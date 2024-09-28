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
        CBasicTextBox*      m_pISUSE;           //���밡�ɿ���
        CBasicTextBox*      m_pSELLER_NAME;
        CBasicTextBox*      m_pExpirationDate;  //�����ð�
        CBasicTextBox*      m_pITEM_COST;       //�ݾ�
        CBasicTextBox*      m_pTurnNum;         //�ȸ� ����
       
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
    // ����Ǵ� �ؽ�Ʈ��
    void                SetRegistCountText( UINT nNow, UINT nMax, UINT nExtended );
    void                SetSaleCommissionText( float fRate );
    void                SetPeeText( const SNATIVEID ItemID );
    void                SetPeeGameMoneyText( unsigned int nIndex );
    // �ǸŵǴ� ������ �̹���
	void				SetImage( const SITEMCUSTOM& sItemCustom );
    void                SetImage( const SNATIVEID ItemID, WORD wTurnNum );
    // �ǸŵǴ� ������ ���� �ؽ�Ʈ ����
    void                ResetSaleCountBox();
    void                ResetSalePriceBox();

    bool                RegistCheck();


    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;
    Pagenavigation*     m_pPagenavigation;
    MarketRPPageRow*    m_pRow[emRowSize];
    CBasicLineBoxSmart* m_pSelectImg;
    CSmartComboBoxBasicEx* m_pPeriodGameMoney;   // �Ǹ� ���ӸӴ�
    CBasicTextBox*      m_pRegistCountText;
    CBasicTextBox*      m_pExtendedHoursText;
    RnButton*           m_pButton;
    CBasicTextBox*      m_pSaleCommissionText;  // �Ǹ� ������
    CBasicTextBox*      m_pRegistValue;         // ��� ���
    CBasicTextBox*      m_pMyMoneyText;         // ���� �Ӵ�
    CBasicTextBox*      m_pMyPointText;         // ���� ����Ʈ
    CSmartComboBoxBasicEx* m_pPeriodComboBox;   // �Ǹ� ��Ͻð�
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

    SINVENITEM          m_sInvenItem;           // ���� ����Ϸ��� ������
};
