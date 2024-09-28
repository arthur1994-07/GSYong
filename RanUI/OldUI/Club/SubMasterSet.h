#pragma	once

#include "../../../EngineLib/GUInterface/UIWindow.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicButton;
class CBasicTextButton;
class GLClubClient;
class GLGaeaClient;

class CSubMasterSet : public CUIWindow
{
protected:
	enum
	{
		SUBMASTER_OK = ET_CONTROL_NEXT,
		SUBMASTER_CANCEL,

		SUBMASTER_JOIN_BUTTON,
		SUBMASTER_STRIKE_BUTTON,
		SUBMASTER_NOTICE_BUTTON,
		SUBMASTER_MARK_BUTTON,
		SUBMASTER_CERTIFY_BUTTON,
		SUBMASTER_CDM_BUTTON,
	};

public:
	CSubMasterSet(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CSubMasterSet();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CBasicButton* m_pJoinButton;
    CBasicButton* m_pStrikeButton;
    CBasicButton* m_pNoticeButton;
    CBasicButton* m_pMarkButton;
    CBasicButton* m_pCertifyButton;
    CBasicButton* m_pCDMButton;

    CBasicTextBox* m_pTextBox;
    CBasicTextBox* m_pTextCDM;

    DWORD m_dwClubFlag;

    DWORD m_dwCharID; // 부클럽마스터의 캐릭터 ID
    GLClubClient* m_pMyClub;

public:
	void CreateSubControl();

private:
	CBasicButton* CreateFlipButton( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );
	CBasicTextButton* CreateTextButton( char* szButton, UIGUID ControlID , char* szText );

public:
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void LoadCurrentOption( DWORD dwCharID );
	void SetSubMasterName( const CString& strName );

private:
	void LoadJoin();
	void LoadStrike();
	void LoadNotice();
	void LoadMark();
	void LoadCertify();
	void LoadClubDeathMatch();
};