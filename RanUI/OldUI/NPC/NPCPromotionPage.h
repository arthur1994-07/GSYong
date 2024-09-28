#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CBasicTextBox;
class RnButton;

class CNPCPromotionPage : public CUIGroup
{
private:
    enum
    {
        PROMOTION_BACK = NO_ID + 1,
        BUY_BUTTON = PROMOTION_BACK + 1,
        ITEM_IMAGE = PROMOTION_BACK + 2,
    };

public:
    CNPCPromotionPage( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CNPCPromotionPage();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CBasicLineBoxSmart* m_pPromotionBack;
    CItemImage* m_pPromotionItem;
    CBasicLineBoxSmart* m_pPromotionTextBack;
    CBasicTextBox* m_pPromotionText;
    RnButton* m_pBuyButton;

public:
    void CreateSubControl();

    void SetRecommendItemImage();
    void PointshopOpenPromotionItem();
};