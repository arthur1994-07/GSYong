#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"

const INT nMAX_PETSTYLE = 20;

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;

class CPetStyleCard : public CUIWindowEx
{
	enum
	{
		PETSTYLECARD_LEFT = ET_CONTROL_NEXT,
		PETSTYLECARD_RIGHT,

		PETSTYLECARD_OK,
		PETSTYLECARD_CANCEL,
	};

public:
	CPetStyleCard(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPetStyleCard();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl* CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

    CUIControl* CreateControlFromPetCommentLua( int nIndex );

    int GetPetStyleIndex();

private:
	CBasicTextBox * m_pStyleName;
    std::vector<CUIControl*> m_vecPetStyle;
	int m_emTYPE;

    std::string m_pId2GameText;

	WORD m_wPet;
	WORD m_wPetBack;

	BOOL m_bOKButton;

};