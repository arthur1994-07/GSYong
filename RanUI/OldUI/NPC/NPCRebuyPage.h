#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../Util/NpcActionable.h"

class CBasicLineBoxSmart;
class CBasicScrollBarEx;
class CRebuyListNode;
class CBasicTextBox;

static const int MAX_REBUY_LIST_NODE = 18;

class CNPCRebuyPage : public CUIGroup, private CUIControlNpcActionable
{
private:
    static const int s_nRebuyListColumnNum;
    static const int s_nRebuyListLinePerOneView;

private:
    enum
    {
        REBUY_PAGE_BACK = NO_ID + 1,
        REBUY_LIST_SCROLLBAR = REBUY_PAGE_BACK + 1,

        REBUY_LIST_NODE_0 = REBUY_PAGE_BACK + 2,
        REBUY_LIST_NODE_END = REBUY_LIST_NODE_0 + MAX_REBUY_LIST_NODE - 1,

        CHECK_REBUY_SUPPLIES = REBUY_LIST_NODE_END + 1,
        CHECK_ALL_DEL_ITEMS = CHECK_REBUY_SUPPLIES + 1,

        CHECK_SELL_R_BTN = CHECK_REBUY_SUPPLIES + 2,
        CHECK_CONFIRM_SELL_ITEM = CHECK_REBUY_SUPPLIES + 3,
    };

public:
    CNPCRebuyPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CNPCRebuyPage();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    RebuyItem::REBUY_VEC m_vecRebuy;

    int m_nTotalLine;
    int m_nRebuyListStartIndex;

private:
    CBasicLineBoxSmart* m_pRebuyPageBack;
    CBasicScrollBarEx* m_pListScrollBar;

    CRebuyListNode* m_pRebuyListNode[ MAX_REBUY_LIST_NODE ];
    CRebuyListNode* m_pRebuyListNodeLeftTemp;
    CRebuyListNode* m_pRebuyListNodeRightTemp;

    CBasicTextBox* m_pNotifyAllDelItems;

    CUIControl* m_pCheckRebuySupplies;
    CUIControl* m_pUncheckRebuySupplies;
    CBasicTextBox* m_pRebuySuppliesText;

    CUIControl* m_pCheckAllDelItems;
    CUIControl* m_pUncheckAllDelItems;
    CBasicTextBox* m_pAllDelItemsText;

    CUIControl* m_pCheckSellRBtn;
    CUIControl* m_pUncheckSellRBtn;
    CBasicTextBox* m_pSellRBtnText;

    CUIControl* m_pCheckConfirmSell;
    CUIControl* m_pUncheckConfirmSell;
    CBasicTextBox* m_pConfirmSellText;

public:
    void CreateSubControl();

    void SetNPCID( const SNATIVEID sNPCID, const DWORD dwNPCGlobalID );

    void LoadRebuyListNodes();

    void LoadRebuyList();

    int GetRebuyListCount();

    void SetCheckRebuySupplies( bool bCheck );

private:
    void SetCheckAllDelItems( bool bCheck );

    void SetCheckSellRBtnItem( bool bCheck );
    void SetCheckConfirmSellItem( bool bCheck );
};