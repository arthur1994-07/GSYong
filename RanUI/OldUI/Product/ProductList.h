#pragma once

#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
//#include "../../../RanLogic/Item/GLItemMixMan.h"
#include "../../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../InnerInterface.h"

class GLGaeaClient;
class CBasicScrollBarEx;
class CBasicTextBoxEx;

class CProductList : public CBasicLineBoxSmart
{
private:
    enum
    {
        START_ID = NO_ID + 1,
        ID_PRODUCT_TYPE_LIST = START_ID,
        ID_PRODUCT_TYPE_LIST_TEXTLIST,
        ID_PRODUCT_TYPE_LIST_SCROOL,
    };
    
public:
    CProductList( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface );

	void					CreateSubControl();
	virtual void			Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual void			TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

    void					SetSeletedTabID( UIGUID ControlID );
    void					SetFilter( BYTE eFindClass, BOOL bSetIsPossible );
	SProductRecipe*			Select( int nIndex );
	void					ResetTextBox();
	void					FilteringTextBox();
	int						GetSelectPos() { return m_nSelectedPos; }

protected:
	GLGaeaClient*			m_pGaeaClient;
    CInnerInterface*		m_pInterface;

	CBasicScrollBarEx*		m_pScrollBar;
    CBasicTextBoxEx*		m_pTextBox;
    CBasicLineBoxSmart*		m_pSeleted;
    UIRECT					m_rcSeleted;
    int						m_nSelectedPos;
    PtrVector_ProductRecipe	m_PtrVectorProductRecipe;
    BYTE					m_eFindSuit;
    BYTE					m_eFindClass;
    BOOL					m_bSetIsPossible;
};