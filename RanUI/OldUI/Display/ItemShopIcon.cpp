#include "StdAfx.h"
#include "ItemShopIcon.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CItemShopIcon::CItemShopIcon(EngineDeviceMan* pEngineDevice) 
    : CUIGroup(pEngineDevice)
    , m_pItemShopImage ( NULL )
    , m_bVisible ( true )
    , m_bUsedMemPool ( false )
    , m_bRender ( false )
{
}

CItemShopIcon::~CItemShopIcon ()
{
}

void CItemShopIcon::INITIALIZE()
{
	m_bVisible = true;
	m_bRender = false;
	m_pItemShopImage->SetVisibleSingle( TRUE );
}

void  CItemShopIcon::CreateSubControl ()
{
	CUIControl* pItemShopImage = new CUIControl(m_pEngineDevice);
	pItemShopImage->CreateSub ( this, "ITEM_SHOP_IMAGE");
	RegisterControl ( pItemShopImage );
	m_pItemShopImage = pItemShopImage;
}