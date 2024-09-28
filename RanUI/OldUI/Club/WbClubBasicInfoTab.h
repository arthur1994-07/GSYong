#pragma	once

#include "../Util/UIWindowEx.h"

class CInnerInterface;
class GLGaeaClient;
class CWbClubInfoField;
class CWbClubBasicInfoGrade;
class CWbClubBasicInfoCommand;
class CWbClubInfoNoticeEdit;
class CWbClubBasicInfoRelation;

class CBasicLineBoxSmart;
class CCheckBox;
class RnButton;

class CWbClubBasicInfoTab : public CUIGroup
{
private:
    static __time64_t tDEFAULT_NOTICE_DATA;

private:
    enum
    {
        BACK = NO_ID + 1,
        CLASS_FIELD,
        LEVEL_FIELD,
        NAME_FIELD,
        CHANNEL_FIELD,
        POSITON_FIELD,
        GRADE_FIELD,
        CLASS_FIELD_LIST,
        LEVEL_FIELD_LIST,
        NAME_FIELD_LIST,
        CHANNEL_FIELD_LIST,
        POSITON_FIELD_LIST,
        GRADE_FIELD_LIST,
        ONLY_ONLINE_CHECK,
        BAN_BTN,
        DELEGATE_BTN,
        CD_AUTHORIZE_BTN,
        CHANGE_GRADE_BTN,
        SET_GRADE_POPUP,
        COMMAND_POPUP,
        NOTICE_EDIT_BTN,
        NOTICE_EDIT,
        RELATION,
    };

    enum
    {
        NUM_RECENT_ACTIVITY = 10,
        ACTIVITY_VISIBLE_LINE = 12,
    };

public:
    CWbClubBasicInfoTab(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~CWbClubBasicInfoTab();

private:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    void CreateSubControl();

    CUIGroup* GetTapWindow(UIGUID controlID);

    void SetClubListOrder( club::ALIGN_ORDER_TYPE Type, club::ALIGN_ORDER_DES_ASC Order );

    void ShowClubMember();

    const DWORD GetClubMemberID();
	const DWORD GetClubAllianceID();

    void ShowGradePopup();
    void ShowNotice();
    void ShowClubLog();
    void LoadClubAllianceList();
    void LoadClubBattleList();

    void KickMember( int ClickIndex = -1 );
    void DelegateMaster( int ClickIndex  = -1 );
    void AuthorizeCdCertifier( int ClickIndex  = -1 );
    void ChangeGrade( int ClickIndex  = -1 );

    void RefreshGuidanceMap();

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID controlID, DWORD dwMsg );
    virtual void SetVisibleSingle(BOOL isVisible);

public:
	// 임시해결
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );

private:
    void AddClubMember( club::GLCLUBMEMBER_CLIENT& sClubMember );
    std::string GetClassString( EMCHARCLASS emClass );

    void SelectMember( int nIndex );
    void CheckShowButtons();

    bool IsSelectedIndex();
    const club::GLCLUBMEMBER_CLIENT& GetSelectMember();

    bool IsCDAuthorizeToMaster();

	/**
		strInput(문자열)을 ByteLent만큼 잘라서 strOutput에 넣어준다.
		클럽 창 기본정보 탭 직업란에 들어갈 문자를
		2byte 크기만큼만 넣어 주기 위해서 만든 함수
	*/
	void StrCutByte( CString& strInput, CString& strOutput, BYTE ByteLent );

private:
    CBasicLineBoxSmart* m_pListBack;

    CWbClubInfoField* m_pClassField;
    CWbClubInfoField* m_pLevelField;
    CWbClubInfoField* m_pNameField;
    CWbClubInfoField* m_pChannelField;
    CWbClubInfoField* m_pPositionField;
    CWbClubInfoField* m_pGradeField;
    CBasicScrollBarEx* m_pListScroll;

    CCheckBox* m_pOnlyOnlineCheck;
    CBasicTextBox* m_pOnlyOnlineText;
    CBasicTextBox* m_pOnOfflineText;
    RnButton* m_pBanBtn;
    RnButton* m_pDelegateBtn;
    RnButton* m_pCDAuthorizeBtn;
    RnButton* m_pChangeGradeBtn;

    CWbClubBasicInfoGrade* m_pSetGradePopup;
    CWbClubBasicInfoCommand* m_pCommandPopup;
    
    CBasicTextBox* m_pNoticeTitle;
    CBasicTextBox* m_pNoticeDate;
    CBasicTextBox* m_pNoticeBody;
    CBasicScrollBarEx* m_pNoticeScrollBar;
    RnButton* m_pNoticeEditBtn;
    CWbClubInfoNoticeEdit* m_pNoticeEdit;

    CBasicTextBox* m_pActivityTitle;
    CBasicTextBox* m_pActivityBody;
    CBasicScrollBarEx* m_pActivityScrollBar;

    CWbClubBasicInfoRelation* m_pRelation;

private:
    std::vector<club::GLCLUBMEMBER_CLIENT> m_vecMembers;

    club::ALIGN_ORDER_TYPE m_OrderType;
    club::ALIGN_ORDER_DES_ASC m_OrderASC_DSC;

    int m_nSelectIndex;

    bool m_bShowCommandPopup;
    UIRECT m_rcSelectPart;
};