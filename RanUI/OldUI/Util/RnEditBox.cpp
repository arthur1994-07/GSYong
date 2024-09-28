#include "StdAfx.h"

#include "RnEditBox.h"


#include "../../../EngineLib/GUInterface/UIEditBox.h"

#include "../../../EngineLib/GUInterface/UIKeyCheck.h"


RnEditBox::RnEditBox( EngineDeviceMan* pEngineDevice ) : CUIEditBoxMan(pEngineDevice)
{

}

void RnEditBox::Create( CUIGroup* pParent,const std::string& editManKey,const std::string& editKey,const std::string& carretKey,UIGUID id, bool isCarratUseRender,DWORD carretColor,CD3DFontPar* pFont,int limitCharacter )
{
	// 		pEditBoxMan->CreateSub( this, "POINT_SHOP_SEARCH_EDIT_MAN", UI_FLAG_DEFAULT, POINT_SHOP_ITEM_SEARCH_MAN );
	// 		pEditBoxMan->CreateEditBox( POINT_SHOP_ITEM_SEARCH_EDIT, "POINT_SHOP_SEARCH_EDIT", "POINT_SHOP_SEARCH_CARRAT", TRUE, UINT_MAX, pFont9, 30 );
	CreateSub(pParent,editManKey.c_str(),UI_FLAG_DEFAULT,id);
	CreateEditBox(EDITBOX_ID,editKey.c_str(),carretKey.c_str(),isCarratUseRender,carretColor,pFont,limitCharacter);

	m_pEditBox = GetEditBox(EDITBOX_ID);
}

void RnEditBox::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIEditBoxMan::Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);

	DWORD dwMsg = MAKE_UIMSG ( 0, RB, MB, LB );
	if( !m_pEditBox->MouseUpdate(x,y) )
	{
		if( CHECK_LB_UP_LIKE(dwMsg) )
		{
			EndEdit();
		}
	}

	if( UIKeyCheck::GetInstance()->Check( DIK_ESCAPE, DXKEY_DOWN )  )
	{
		EndEdit();
	}

	if( m_pEditBox->IsBegin() )
	{
		if( UIKeyCheck::GetInstance()->Check( DIK_RETURN, DXKEY_DOWN ) )
		{
			AddMessageEx(UIMSG_RNEDITBOX_RETURN);
		}

		if( GetEditString() != m_CacheString )
		{
			AddMessageEx(UIMSG_RNEDITBOX_CHANGE);
			m_CacheString = GetEditString();
		}
	}
}

void RnEditBox::SetEditString( const std::string& text )
{
	CUIEditBoxMan::SetEditString(EDITBOX_ID,text.c_str());
}

std::string RnEditBox::GetEditString()
{
	return CUIEditBoxMan::GetEditString(EDITBOX_ID).GetString();
}
