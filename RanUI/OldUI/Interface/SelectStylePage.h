#pragma	once

#include "../Util/UIOuterWindow.h"
#include "NewCreateCharEnum.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class COuterInterface;
class CBasicTextBox;
class CBasicTextButton;
class CUIEditBox;
class GLGaeaClient;

class CSelectStylePage	: public CUIOuterWindow
{
public:
    CSelectStylePage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CSelectStylePage();

private:
	static	const int nLIMITCHAR;

	enum
	{
		SELECT_FACE_LEFT = ET_CONTROL_NEXT,
		SELECT_FACE_RIGHT,
		SELECT_HAIR_LEFT,
		SELECT_HAIR_RIGHT,
		NEWCHAR_NAME_EDIT,
	};

	int					m_nSex;
	int					m_nSchool;
	int					m_nClass;
	int					m_nHair;
	int					m_nFace;

    GLGaeaClient* m_pGaeaClient;
    CBasicTextBox*	m_pSchoolTextBox;
	CBasicTextBox*	m_pClassTextBox;
	CBasicTextBox*  m_pSexTextBox;
	CBasicTextBox*	m_pFaceTextBox;
	CBasicTextBox*	m_pHairTextBox;

//	CBasicTextButton* m_pOkButton;
//	CBasicTextButton* m_CancleButton;

	CUIEditBox*	m_pEditBox;

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	CString m_strClassHair;
	CString m_strClassFace;

public:
    void	SetSelectChar( int nSchool, int nSex, int nClass );
	CUIEditBox*		GetEditBox() { return m_pEditBox; }

	void	ResetAll();

	void	SELECT_STYLE_PROCESS( int nSelect );	
public:
	void	CreateSubControl ();
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

};