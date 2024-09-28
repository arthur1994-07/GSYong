#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CItemSlot;
class CBasicTextBox;
class RnButton;

class CRebuyListNode : public CUIGroup
{
private:
    enum
    {
        
        NODE_BACK = NO_ID + 1,
        NODE_IMAGE = NODE_BACK + 1,
        BUY_BUTTON = NODE_BACK + 2,
    };

public:
    CRebuyListNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CRebuyListNode();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CBasicLineBoxSmart* m_pBack;
    CItemSlot* m_pImage;
    CBasicTextBox* m_pName;
    CBasicTextBox* m_pPrice;

    RnButton* m_pBuyButton;

    CBasicLineBoxSmart* m_pSelectItem;

private:
    SITEMCUSTOM m_sItemCustom;
    LONGLONG m_Price;

    bool m_bVisibleBuyButton;

public:
    void CreateSubControl();

    void SetItem( RebuyItem::SREBUY_ITEM& sRebuyItem );
    void ResetItem();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

private:
    void SetVisibleBuyButton( bool bVisible );

    void BuyRebuyListItem();
};