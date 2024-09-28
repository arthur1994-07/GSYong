#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class CBasicTextBox;
class GLGaeaClient;
class CSwapImage;

class CFaceStyleCard : public CUIWindowEx
{
	enum
	{
		FACESTYLECARD_LEFT = ET_CONTROL_NEXT,
		FACESTYLECARD_RIGHT,

		FACESTYLECARD_OK,
		FACESTYLECARD_CANCEL,

		SEX_W = 0,
		SEX_M = 1,
	};

public:
	CFaceStyleCard ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CFaceStyleCard ();

public:
	void CreateSubControl();

	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextBox * CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CUIControl * CreateControl( const char * szControl ); // Note : 상위 클래스 함수 재정의

public:
	void CalcTexture();

private:
	CBasicTextBox * m_pStyleName;
	char * m_pId2GameText;

	int m_nFace;
	int m_nFaceBack;
	int m_nMaxFace;
	int m_nSex;

	BOOL m_bOKButton;

	CSwapImage*	m_pFaceStyleSw;

protected:
	GLGaeaClient* m_pGaeaClient;

};