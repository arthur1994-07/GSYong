#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/RnButton.h"

class CBasicTextBox;
class CBasicButton;
class CCheckBox;
class CItemImage;
class GLGaeaClient;
class RnButton;

class CUIGroupUtil
{
protected:
	
	CBasicTextBox*	 CreateText( const std::string& text,const std::string& key,WORD wAlignFlag,UIGUID id = NO_ID);
    CBasicTextBox*	 CreateText( const std::string& text,const std::string& key,WORD wAlignFlag,D3DCOLOR d3dColor, UIGUID id = NO_ID);
	CUIControl*      CreateControl( const std::string& key,UIGUID id = NO_ID);
	CUIControl*      CreateOverRect( const std::string& key,UIGUID id = NO_ID);
	CUIControl*      CreateLineBox( const std::string& strControl, const std::string& strTexInfo,UIGUID id = NO_ID );
	CUIControl*      CreateLineBoxChar( const char* strControl, const char* strTexInfo );
	CBasicButton*	 CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID, WORD wFlipType );
	CBasicTextBox*	 CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign );
	CCheckBox*       CreateCheckBox(const std::string& key,UIGUID id,const char* szCheckBoxOnTexture,const char* szCheckBoxOffTexture,bool isCheck);
	CItemImage*		 CreateItemImage( const char* szControl, UIGUID ControlID );
    RnButton*        CreateRnButton( RnButton::CreateArg& arg, const std::string& key, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID id = NO_ID );
    RnButton*        CreateRnButton( const std::string& text, const std::string& key, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID id = NO_ID );

	virtual CUIGroup* GetUIGroupThis() = 0;
	virtual EngineDeviceMan* GetEngineDeviceMan() = 0;
	virtual GLGaeaClient* GetGaeaClient() { GASSERT(false); return NULL; }

};


class CUIGroupHelper : public CUIGroup , public CUIGroupUtil
{
protected:
	CUIGroupHelper(EngineDeviceMan* pEngineDevice) : CUIGroup(pEngineDevice) {}
	virtual CUIGroup* GetUIGroupThis() { return this; }
	virtual EngineDeviceMan* GetEngineDeviceMan() { return m_pEngineDevice; }	
};