#pragma once

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../InnerInterface.h"
//#include "../../../RanLogic/Item/GLItemMixMan.h"
#include "../../../RanLogic/Product/GLProductRecipeMan.h"
class GLGaeaClient;
class CItemImage;
class CBasicTextBox;
class CUIEditBoxMan;

class CProductGoods : public CBasicLineBoxSmart
{
public:
    enum
    {
		LIMIT_COLUMN_CNT = Product::ITEM_PRODUCT_MAX_MATERIAL,
    };

private:
    enum
    {
        ID_PRODUCT_GOODS_RESULT_ITEMSLOT = NO_ID + 1,
        ID_PRODUCT_GOODS_EDITBOX_MANAGER,
        ID_PRODUCT_GOODS_PRODUCT_COUNT_EDIT,
        ID_PRODUCT_GOODS_PRODUCT_COUNT_BOX,
    };
    enum
    {
		// nLIMIT_PRODUCT_CNT개수만큼 할당하기 때문에 시작과 끝을 지정하였다.
        ID_PRODUCT_GOODS_ITEMSLOT_START = ID_PRODUCT_GOODS_RESULT_ITEMSLOT + 1000,
        ID_PROCUCT_GOODS_ITEMSLOT_END   = ID_PRODUCT_GOODS_ITEMSLOT_START + LIMIT_COLUMN_CNT,
        ID_PRODUCT_GOODS_ITEMSLOT_TEXT_START = ID_PROCUCT_GOODS_ITEMSLOT_END + 1,
        ID_PRODUCT_GOODS_ITEMSLOT_TEXT_END = ID_PRODUCT_GOODS_ITEMSLOT_START+LIMIT_COLUMN_CNT,
    };

public:
    CProductGoods( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface );

	void                CreateSubControl();
	virtual void		Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	void				ShowRandomOptionMaterialBorder();
    void                SetItemInformation( SProductRecipe* pProductRecipe );
	void                SetMaterials();

	void                DoStart( bool bOngoing=false );
    void                DoCancel();

    void                Clear();

	SProductRecipe*		GetItemMix() { return m_pProductRecipe; }

protected:
	GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

	CBasicTextBox*      m_pIsProductText;
    CItemImage*	        m_pResulteItemImage;
	CBasicTextBox*      m_pResultItemText;
    CBasicTextBox*      m_pProductTimeText;
    CItemImage*	        m_pItemImageArray[LIMIT_COLUMN_CNT];	
    CBasicTextBox*      m_pMaterialText[LIMIT_COLUMN_CNT];
    unsigned int        m_nMaterialCount[LIMIT_COLUMN_CNT];
    CBasicTextBox*      m_pProductMessageText;
    CBasicTextBox*      m_pProductCountText;
    CUIEditBoxMan*      m_pProductEditBox;
    unsigned int        m_nProductCount;
    CBasicTextBox*      m_pCostSumText;
    CBasicTextBox*      m_pMoneyText;

    SProductRecipe*     m_pProductRecipe;

	//hsshin 숙련도 시스템 추가
	CBasicTextBox*      m_pReqLevelText;
	CBasicTextBox*      m_pReqSkillText;
	CBasicTextBox*      m_pGainSkillText;
};