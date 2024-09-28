#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;
class CSwapImage;

class CHairStyleCard : public CUIWindowEx
{
	enum
	{
		HAIRSTYLECARD_LEFT = ET_CONTROL_NEXT,
		HAIRSTYLECARD_RIGHT,

		HAIRSTYLECARD_OK,
		HAIRSTYLECARD_CANCEL,

		SEX_W = 0,
		SEX_M = 1,
	};

public:
	CHairStyleCard(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CHairStyleCard ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

public:
	void CalcTextureHair();

private:
	CBasicTextBox * m_pStyleName;
	// CUIControl * m_pHairStyle;
	CSwapImage*	m_pHairStyleSw;
	char * m_pId2GameText;
	int m_nHair;
	int m_nHairBack;
	int	m_nMaxHair;
	int m_nSex;
	BOOL m_bOKButton;

protected:
	GLGaeaClient* m_pGaeaClient;

};
