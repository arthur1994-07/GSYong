#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class CInnerInterface;
class GLGaeaClient;

class CItemSlot;
class CBasicScrollBarEx;
class GLInventory;
struct SINVENITEM;

class CWbClubStoragePage : public CUIGroup
{
private:
    static const int nSTARTLINE;
    static const int nOUTOFRANGE;
    static const int nENABLE_ALPHA;
    static const int nDISABLE_ALPHA;

protected:
    enum
    {
        nMAX_ONE_VIEW_SLOT = 8
    };

    enum
    {
        ITEM_SLOT0 = NO_ID + 1,
        ITEM_SLOT1,
        ITEM_SLOT2,
        ITEM_SLOT3,
        ITEM_SLOT4,
        ITEM_SLOT5,
        ITEM_SLOT6,
        ITEM_SLOT7,
        ITEM_SLOT_END = ITEM_SLOT0 + EStorageY
    };

public:
    CWbClubStoragePage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubStoragePage();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void InitStoragePage( int nPage );

public:
    void SetItemIndex( int nPosX, int nPosY );
    void GetItemIndex( int* pnPosX, int* pnPosY );

public:
    void LoadItemPage(GLInventory* pItemData, const bool bUSABLE = false);
    void LoadItem( SINVENITEM& ref_InvenItem );

    const SINVENITEM* GetItem(int nPosX, int nPosY) const;

public:
    void UnLoadItemPage ();
    void UnLoadItem( int nPosX, int nPosY );

private:
    void ResetAllItemSlotRender( int nTotal );
    void SetItemSlotRender( int nStartIndex, int nTotal );

    void GetSplitPos ( WORD* pwPosX, WORD* pwPosY );
    void SetSplitPos ( WORD wPosX, WORD wPosY );

    void SetDiffuseAlphaAllControls( int nAlpha );

private:
    CItemSlot* CreateItemSlot( CString strKeyword, UIGUID ControlID );
    CUIControl*	CreateControl( const char* szControl );

private:
    CBasicLineBoxSmart* m_pBack;
    CBasicTextBox* m_pLabel;
    CItemSlot* m_pItemSlotArray[EStorageY];
    CUIControl*	m_pItemSlotArrayDummy[nMAX_ONE_VIEW_SLOT];

private:
    int m_nPage;
    bool m_bSplitItem;

    bool m_bEnable;

private:
    int m_nCurPos;
    int	m_nPosX;
    int	m_nPosY;
    int m_wSplitItemPosX;
    int m_wSplitItemPosY;

};