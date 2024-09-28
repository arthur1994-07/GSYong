#pragma once

#include "ItemInfoTooltip.h"

// itemInfoTooltipGroup [4/15/2013 hsshin];
// ������ �������â�� ���ÿ� ������ ���� ���� �����ִ� Ŭ����;
// ���� GUID�� ������ �Է��ϸ� pool���� ���� �������â�� �̿��� â�� ����.;
// �ִ� ��� ������ �Ѿ�� ���̻� ����� ����.;

class CItemToolTipGroup
{

private:

    struct SItemToolTip
    {
        UIGUID nID;
        CItemInfoTooltip* pControl;
    };

    typedef std::vector< SItemToolTip > ItemToolTipVec;
    typedef ItemToolTipVec::iterator ItemToolTipVecIter;

    ItemToolTipVec m_vecItemToolTipRegist;
    ItemToolTipVec m_vecItemToolTipPool;
    ItemToolTipVec m_vecItemToolTipUsed;

    CInnerInterface* m_pInterface;

    UIGUID m_BeginGUID;

    CItemInfoTooltip* const CItemToolTipGroup::ShowItemToolTip( const SITEMCUSTOM& sItemCustom,
        const BOOL bShopOpen, const BOOL bInMarket,
        const BOOL bInPrivateMarket, const BOOL bUsePreview,
        const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
        SNATIVEID sNpcNativeID, int iPageIndex = -1 );
public:
    CItemToolTipGroup( CInnerInterface* _pInterface, GLGaeaClient* _pGaeaClient, EngineDeviceMan* _pEngineDevice, UIGUID iBeginGUID, int iNumTooltipMulti );
    virtual ~CItemToolTipGroup() {};

    BOOL IsShowItemToolTipMulti();

    CItemInfoTooltip* const ShowItemToolTipMulti( const SITEMCUSTOM& sItemCustom,
        const BOOL bShopOpen, const BOOL bInMarket,
        const BOOL bInPrivateMarket, const BOOL bUsePreview,
        const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
        SNATIVEID sNpcNativeID, int iPageIndex = -1 );

    CItemInfoTooltip* const ShowItemToolTipMulti( WishList::ItemSPtr pWishItem,
        const BOOL bShopOpen, const BOOL bInMarket,
        const BOOL bInPrivateMarket, const BOOL bUsePreview,
        const BOOL bIsWEAR_ITEM, WORD wPosX, WORD wPosY,
        SNATIVEID sNpcNativeID );

    CItemInfoTooltip* const ShowItemToolTipMulti( WishList::ItemSPtr pWishItem, int x, int y );
    CItemInfoTooltip* const ShowItemToolTipMulti( SITEMCUSTOM& sItemCustom, int x, int y );
	CItemInfoTooltip* const ShowItemToolTipMulti( SITEMCUSTOM& sItemCustom, int x, int y, SNATIVEID sNpcNativeID, int iPageIndex = -1);

    void CloseItemToolTipMulti( UIGUID ControlID );
    void CloseItemToolTipMultiAll();
    void TranslateMessage( UIGUID ControlID, DWORD dwMsg );
    void Clear();
};