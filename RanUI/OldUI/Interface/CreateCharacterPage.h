#pragma	once

#include "../Util/UIOuterWindow.h"
#include "../../../RanLogic/Character/GLCharDefine.h"

class COuterInterface;
class CUIEditBox;
class CBasicButton;
class CBasicTextBox;
class CBasicTextButton;
class GLGaeaClient;

class CCreateCharacterPage : public CUIOuterWindow
{
private:
static	const int nLIMITCHAR;

private:
	enum
	{
		CREATE_PAGE_OK = ET_CONTROL_NEXT,
		CREATE_PAGE_CANCEL,
		CREATE_SCHOOL_L,
		CREATE_SCHOOL_R,
		CREATE_CLASS_L,
		CREATE_CLASS_R,
		CREATE_FACE_L,
		CREATE_FACE_R,
		CREATE_HAIR_L,
		CREATE_HAIR_R,
		CREATE_PAGE_NAME_EDIT,		
	};

public:
	CCreateCharacterPage(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCreateCharacterPage ();

	void CreateSubControl ();

	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

	void ResetAll();

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

	BOOL	CheckString( CString strTemp );

private:
    GLGaeaClient* m_pGaeaClient;
	CUIEditBox*	m_pEditBox;

	CBasicTextBox*	m_pSchoolTextBox;
	CBasicTextBox*	m_pClassTextBox;
	CBasicTextBox*	m_pFaceTextBox;
	CBasicTextBox*	m_pHairTextBox;

	CBasicTextButton* m_pOKButton;

	BOOL m_bUseCancel;
	WORD m_wClassMax;

	CString m_strClassHair;
	CString m_strClassFace;

public:
	void UseCancelButton()				{ m_bUseCancel = TRUE; }
};