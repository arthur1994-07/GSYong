#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "NewCreateCharEnum.h"

struct SSelectImage
{
    CUIControl* m_pTip;
    CUIControl* m_pOn;
    CUIControl* m_pOff;
    CUIControl* m_pFocus;
};

class COuterInterface;
class CUIControl;
class CBasicTextBox;
class CBasicButton;
class CUIEditBox;
class GLGaeaClient;

class CCreateCharacterMenu : public CUIGroup
{
private:
    static	const int nLIMITCHAR;

private:
    enum
    {
        CREATE_SCHOOL_FOCUS_0 = NO_ID + 1,
        CREATE_SCHOOL_FOCUS_1,
        CREATE_SCHOOL_FOCUS_2,

        CREATE_SEX_FOCUS_0,
        CREATE_SEX_FOCUS_1,

        CREATE_MALE_FOCUS_0,
        CREATE_MALE_FOCUS_1,
        CREATE_MALE_FOCUS_2,
        CREATE_MALE_FOCUS_3,
        CREATE_MALE_FOCUS_4,
        CREATE_MALE_FOCUS_5,
        CREATE_MALE_FOCUS_6,
        CREATE_MALE_FOCUS_7,
		CREATE_MALE_FOCUS_8,

        CREATE_FEMALE_FOCUS_0,
        CREATE_FEMALE_FOCUS_1,
        CREATE_FEMALE_FOCUS_2,
        CREATE_FEMALE_FOCUS_3,
        CREATE_FEMALE_FOCUS_4,
        CREATE_FEMALE_FOCUS_5,
        CREATE_FEMALE_FOCUS_6,
        CREATE_FEMALE_FOCUS_7,
		CREATE_FEMALE_FOCUS_8,

        CREATE_SCHOOL_TIP_0,
        CREATE_SCHOOL_TIP_1,
        CREATE_SCHOOL_TIP_2,

        CREATE_SEX_TIP_0,
        CREATE_SEX_TIP_1,

        CREATE_MALE_TIP_0,
        CREATE_MALE_TIP_1,
        CREATE_MALE_TIP_2,
        CREATE_MALE_TIP_3,
        CREATE_MALE_TIP_4,
        CREATE_MALE_TIP_5,
        CREATE_MALE_TIP_6,
        CREATE_MALE_TIP_7,
		CREATE_MALE_TIP_8,

        CREATE_FEMALE_TIP_0,
        CREATE_FEMALE_TIP_1,
        CREATE_FEMALE_TIP_2,
        CREATE_FEMALE_TIP_3,
        CREATE_FEMALE_TIP_4,
        CREATE_FEMALE_TIP_5,
        CREATE_FEMALE_TIP_6,
        CREATE_FEMALE_TIP_7,
		CREATE_FEMALE_TIP_8,

        CREATE_FACE_LEFT,
        CREATE_FACE_RIGHT,
        CREATE_HAIR_LEFT,
        CREATE_HAIR_RIGHT,
        CREATE_NAME_EDIT,
    };

    enum
    {
        EXTREME,
        SWORDMAN,
        ARCHER,
        BRAWLER,
        SHAMAN,
        SCIENTIST,
        ASSASSIN,
		TRICKER,
        ETC,
    };


public:
    CCreateCharacterMenu(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CCreateCharacterMenu();

    void CreateSubControl ();

    CUIControl*	CreateControl( CString strControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
    CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
    CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );

    void ResetData();
    void ResetEdit();
    void SelectIndex();
    int GetCharIndex();
    void CreateCharProcess();
    BOOL CheckStringName( CString strTemp );
    void ResetAll();
    BOOL IsCancelButton() { return m_bUseCancel; }
    void UseCancelButton() { m_bUseCancel = TRUE; }

    CUIEditBox* GetEditBox() { return m_pEditBox;}

    BOOL GetMenuSelect() { return m_bSelect; }	

    void GetSelectInfo(INT& nSelectSchool, INT& nSelectSex, INT& nSelectClass );

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle ( BOOL bVisible );

private:
    SSelectImage m_sSchool[ MAX_SELSCHOOL ];

    SSelectImage m_sSex[ MAX_SEX ];

    SSelectImage m_sMale[ MAX_CLASS ];
    SSelectImage m_sFemale[ MAX_CLASS ];

    CBasicTextBox*	m_pFaceTextBox;
    CBasicTextBox*	m_pHairTextBox;

    CBasicTextBox*	m_pSchoolTextBox;
    CBasicTextBox*	m_pSexTextBox;
    CBasicTextBox*	m_pClassTextBox;
    CBasicTextBox*	m_pSetTextBox;

    CUIEditBox*	m_pEditBox;

    INT m_nSelectSchool;
    INT m_nSelectSex;
    INT m_nSelectClass;
    BOOL m_bSelect;
	BOOL m_bExtreme;

    BOOL m_bUseCancel;

protected:
    COuterInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};