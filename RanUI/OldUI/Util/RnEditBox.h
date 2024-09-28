
#pragma	once

class CUIEditBox;


#include "../../../EngineLib/GUInterface/UIEditBoxMan.h"


const DWORD UIMSG_RNEDITBOX_RETURN = UIMSG_USER1;
const DWORD UIMSG_RNEDITBOX_CHANGE = UIMSG_USER2;

class RnEditBox : public CUIEditBoxMan
{
	static const int EDITBOX_ID = 10;
public:
	RnEditBox(EngineDeviceMan* pEngineDevice);

	void Create(CUIGroup* pParent,const std::string& editManKey,const std::string& editKey,const std::string& carretKey,UIGUID id,
		bool isCarratUseRender,DWORD carretColor,CD3DFontPar* pFont,int limitCharacter);



	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	void SetEditString(const std::string& text);
	std::string GetEditString();

private:
	CUIEditBox* m_pEditBox;
	std::string m_CacheString;
};

