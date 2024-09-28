#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItem.h"

//-----------------------------------------------------------------------------------------------//

#include <vector>

//-----------------------------------------------------------------------------------------------//

class GLGaeaClient;
class CInnerInterface;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CItemImageEx;
class TaskN;

//-----------------------------------------------------------------------------------------------//

class CAttendanceAward : public CUIGroup
{

public:
    CAttendanceAward(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CAttendanceAward();

public:
    enum 
    {
        ATTENDANCE_AWARD = NO_ID,
        ATTENDANCE_AWARD_ITEMSLOT,
    };

public:
    enum EMSTATE
    {
        EMSTATE_ENABLE,
        EMSTATE_DISABLE,
        EMSTATE_COMPLETE,
        EMSTATE_EMPTY,
    };

public:
    struct SITEMSLOT : public CUIGroup
    {
        CItemImageEx*       ITEM;
        SITEMCUSTOM         ITEMDATA;
        CBasicLineBoxSmart* LINEBOX;
        GLGaeaClient*       GAEACLIENT;
        CInnerInterface*    INNERINTERFACE;
        bool                ENABLE;

        enum { ID_ITEM = NO_ID+1 };
        SITEMSLOT(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) 
            : CUIGroup(pEngineDevice)
            , ITEM(NULL)
            , LINEBOX(NULL)
            , GAEACLIENT(pGaeaClient)
            , INNERINTERFACE(pInterface)
            , ENABLE(false)
        {
        }
        void CreateSubControl ();
        void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
        void SetEnable ( bool bEnable );
        void Reset ();
    };

public:
    typedef std::vector<SITEMSLOT*> ITEMVEC;
    typedef ITEMVEC::iterator       ITEMVEC_ITER;

public:
    void	        CreateSubControl ();           

public:
    virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
    void            Update ( TaskN* pTask );

public:
    void            SetState ( const EMSTATE emState );

private:
    void            UpdateState ();

private:
    void            SetPoint ( const UINT nEXP, const UINT nActivity, const ULONGLONG llMoney );

private:
    void            ResetItem  ();
    const bool      AddItem    ( const SNATIVEID sItemID );
    const UINT      GetNumItem ();

private:
    SITEMSLOT*      CreateItemSlot ( const UINT nIndex );

private:
    CBasicTextBox*  m_pAward;
    CBasicTextBox*  m_pAwardItem;
    ITEMVEC         m_ItemList;

private:
    UINT            m_nEXP;
    UINT            m_nActivity;
    ULONGLONG       m_llMoney;

private:
    UINT            m_nNumItem;
    EMSTATE         m_emState;

private:
    GLGaeaClient*    m_pGaeaClient;
    CInnerInterface* m_pInterface;

};
