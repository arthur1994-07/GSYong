

#include "stdafx.h"

#include "UIGroupHelper.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../enginelib/GUInterface/BasicLineBoxSmart.h"
#include "../../../enginelib/GUInterface/BasicTextBox.h"
#include "../../../enginelib/GUInterface/BasicButton.h"
#include "../../../enginelib/GUInterface/UITextControl.h"

#include "../Util/CheckBox.h"
#include "../Item/ItemImage.h"

#include "../../../enginelib/DxTools/DxFontMan.h"

CBasicTextBox* CUIGroupUtil::CreateText( const std::string& text,const std::string& key,WORD wAlignFlag,UIGUID id )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	CBasicTextBox* pText =	new CBasicTextBox(GetEngineDeviceMan());
	pText->CreateSub( GetUIGroupThis(), key.c_str(),UI_FLAG_DEFAULT,id);
	pText->SetFont( pFont );
	pText->SetTextAlign( wAlignFlag );
	pText->SetText(text.c_str());
	GetUIGroupThis()->RegisterControl( pText );
	return pText;
}

CBasicTextBox* CUIGroupUtil::CreateText( const std::string& text,const std::string& key,WORD wAlignFlag,D3DCOLOR d3dColor, UIGUID id /*= NO_ID*/ )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    CBasicTextBox* pText =	new CBasicTextBox(GetEngineDeviceMan());
    pText->CreateSub( GetUIGroupThis(), key.c_str(),UI_FLAG_DEFAULT,id);
    pText->SetFont( pFont );
    pText->SetTextAlign( wAlignFlag );
    pText->SetText( text.c_str(), d3dColor );
    GetUIGroupThis()->RegisterControl( pText );

    return pText;
}

CUIControl* CUIGroupUtil::CreateOverRect( const std::string& key,UIGUID id )
{
	CUIControl* overRect = CreateControl(key,id);
	overRect->SetDiffuse(D3DCOLOR_ARGB(125,0,0,0));
	overRect->SetUseRender(false);
	return overRect;
}


CUIControl* CUIGroupUtil::CreateLineBox( const std::string& strControl, const std::string& strTexInfo ,UIGUID id )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
	pLineBox->CreateSub( GetUIGroupThis(), strControl.c_str() ,UI_FLAG_DEFAULT,id);
	pLineBox->CreateSubControl( strTexInfo.c_str() );
	GetUIGroupThis()->RegisterControl( pLineBox );
	return pLineBox;
}


CUIControl* CUIGroupUtil::CreateLineBoxChar( const char* strControl, const char* strTexInfo )
{
	CBasicLineBoxSmart* pLineBox;
	pLineBox = new CBasicLineBoxSmart(GetEngineDeviceMan());
	pLineBox->CreateSub( GetUIGroupThis(), strControl ,UI_FLAG_DEFAULT,NO_ID);
	pLineBox->CreateSubControl( strTexInfo );
	GetUIGroupThis()->RegisterControl( pLineBox );
	return pLineBox;
}

CUIControl* CUIGroupUtil::CreateControl( const std::string& key,UIGUID id )
{
	CUIControl* pControl = new CUIControl(GetEngineDeviceMan());
	pControl->CreateSub(GetUIGroupThis(),key.c_str(),UI_FLAG_DEFAULT,id);
	GetUIGroupThis()->RegisterControl(pControl);
	return pControl;
}

CBasicButton* CUIGroupUtil::CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType )
{
	CBasicButton* pButton = new CBasicButton(GetEngineDeviceMan());
	pButton->CreateSub ( GetUIGroupThis(), szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, wFlipType );
	GetUIGroupThis()->RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CUIGroupUtil::CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(GetEngineDeviceMan());
	pStaticText->CreateSub ( GetUIGroupThis(), szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );
	GetUIGroupThis()->RegisterControl ( pStaticText );
	return pStaticText;
}

CCheckBox* CUIGroupUtil::CreateCheckBox( const std::string& key,UIGUID id,const char* szCheckBoxOnTexture,const char* szCheckBoxOffTexture,bool isCheck )
{
	CCheckBox* pCheckBox = new CCheckBox( GetEngineDeviceMan() ); 
	pCheckBox->CreateSub( GetUIGroupThis(), key.c_str(), UI_FLAG_DEFAULT , id);
	pCheckBox->CreateSubControl ( szCheckBoxOnTexture, szCheckBoxOffTexture);
	pCheckBox->SetCheck( isCheck );
	GetUIGroupThis()->RegisterControl( pCheckBox );
	return pCheckBox;
}

CItemImage* CUIGroupUtil::CreateItemImage(  const char* szControl, UIGUID ControlID  )
{
	CItemImage* pItemImage = new CItemImage(GetGaeaClient(), GetEngineDeviceMan());
	pItemImage->CreateSub ( GetUIGroupThis(), szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl();
	GetUIGroupThis()->RegisterControl ( pItemImage );
	return pItemImage;
}

RnButton* CUIGroupUtil::CreateRnButton( RnButton::CreateArg& arg, const std::string& key, WORD wAlignFlag, UIGUID id /*= NO_ID */ )
{
    RnButton* pButton = new RnButton( GetEngineDeviceMan() );
    pButton->CreateSub( GetUIGroupThis(), key.c_str(), wAlignFlag, id );
    pButton->CreateSubControl(arg);
    GetUIGroupThis()->RegisterControl( pButton );

    return pButton;
}

RnButton* CUIGroupUtil::CreateRnButton( const std::string& text, const std::string& key, WORD wAlignFlag, UIGUID id /*= NO_ID */ )
{
    CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    RnButton::CreateArg ButtonDefault;
    {
        ButtonDefault.defaultTextureKey     = "RNBUTTON_DEFAULT_TEXTURE";
        ButtonDefault.mouseOverTextureKey   = "RNBUTTON_OVER_TEXTURE";
        ButtonDefault.mouseClickTextureKey  = "RNBUTTON_CLICK_TEXTURE";
        ButtonDefault.pFont                 = pFont;
        ButtonDefault.dwColor               = NS_UITEXTCOLOR::WHITE;
        ButtonDefault.nTextAlign            = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;
        ButtonDefault.text                  = text;
    }    

    return CreateRnButton( ButtonDefault, key, wAlignFlag, id );
}
