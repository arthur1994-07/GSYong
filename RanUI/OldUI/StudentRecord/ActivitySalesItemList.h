#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Item/GLItem.h"

//-----------------------------------------------------------------------------------------------//

#include <vector>

//-----------------------------------------------------------------------------------------------//

class  GLGaeaClient;
class  CInnerInterface;
class  CBasicTextBox;
class  CItemImageEx;
class  CBasicScrollBarEx;
class  GLInventory;
struct SACTIVITYSALESITEM;

//-----------------------------------------------------------------------------------------------//

class CActivitySalesItemList : public CUIGroup
{

public:
    CActivitySalesItemList(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CActivitySalesItemList ();

public:
    struct SITEMINFO
    {
        SITEMCUSTOM sITEM;
        LONGLONG    llPRICE;

        SITEMINFO ( const SITEMCUSTOM& sItemCustom, const LONGLONG llPrice )
            : sITEM(sItemCustom), llPRICE(llPrice) {}
    };

public:
    struct SITEMSLOT : public CUIGroup
    {
        CItemImageEx*       ITEM;
        CBasicTextBox*      ITEMNAME;
        CBasicTextBox*      PRICE;
        CBasicTextBox*      REQPOINT;
        SITEMINFO*          ITEMDATA;
        GLGaeaClient*       GAEACLIENT;
        CInnerInterface*    INNERINTERFACE;

        enum { ID_ITEM = NO_ID+1 };
        SITEMSLOT(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
            : CUIGroup(pEngineDevice)
            , ITEM(NULL)
            , ITEMDATA(NULL)
            , ITEMNAME(NULL)
            , PRICE(NULL)
            , REQPOINT(NULL)
            , GAEACLIENT(pGaeaClient)
            , INNERINTERFACE(pInterface)
        {
        }

        void CreateSubControl ();
        void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
        void Reset ();
        void SetItem ( SITEMINFO* pItemInfo );
    };

public:
    typedef std::vector<SITEMSLOT*>  CSLOTVEC;
    typedef CSLOTVEC::iterator       CSLOTVEC_ITER;

    typedef std::vector<SITEMINFO>   CITEMVEC;
    typedef CITEMVEC::iterator       CITEMVEC_ITER;

public:
    enum 
    {
        SALESITEMLIST_NONE = NO_ID,
        SALESITEMLIST_LIST
    };

    enum
    {
        MAX_SLOT = 8,
    };

public:
    void	            CreateSubControl ();           

public:
    virtual	void	    Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	    TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void                ClearList  ();
    void                UpdateList ();
    void                AddList ( SACTIVITYSALESITEM* pSaleData );       

private:

private:
    SITEMSLOT*          CreateSlot ( const UINT nIndex ); 

private:
    CBasicScrollBarEx*   m_pScroll;
    CSLOTVEC             m_SlotList;
    CITEMVEC             m_ItemList;

private:
    UINT                 m_nScrollPos;
    SACTIVITYSALESITEM*  m_pSalesData;

private:
    GLGaeaClient*        m_pGaeaClient;
    CInnerInterface*     m_pInterface;

};
