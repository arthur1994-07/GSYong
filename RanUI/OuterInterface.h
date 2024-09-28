#pragma	once

#include "../SigmaCore/gassert.h"
#include "../EngineLib/GUInterface/UIManCommon.h"
#include "../RanLogic/WorldBattle/WorldBattleDefine.h"
#include "../RanLogicClient/OutInterfaceBase.h"
//#include "../RanLogicClient/InterfaceBaseDefine.h"
#include "../EngineLib/GUInterface/InterfaceBaseDefine.h"

class CUIControl;

class CModalWindow;
class CSelectServerPage;
class CLoginPage;
class CPassKeyboard;
class CSelectCharacterPage;
class CCreateCharacterPage;
class CSelectCharacterLeftPage;
class CSelectCountryUI;
class CSelectContinentListUI;
class CSelectCountryListUI;
class CWaitDialogue;
class DxLobyStage;
class CSecPassSetPage;
class CSecPassCheckPage;
class CMessageWindow;
class CNewCreateCharPage;
class CCreateCharacterWindow;
class CCreateChatacterButtonPage;
class CBasicVarTextBox;
class CBasicTextBox;
class CRotateCharacter;
class GLGaeaClient;
class CLockSelectCharacterPreviewPage;
class CSelectCountryConfirmModal;
class CSelectCountryWindow;
class CSelectClubWindow;

class COuterInterface : public OutInterfaceBase, public ITranslateUIMsg
{
	static	const	float	fLIST_LINE_INTERVAL;

private:
    BOOL	m_bOnTheInterface;
	bool	m_bBlockProgramFound;
	float	m_fWaitTime;
	BOOL	m_bUseWaitTime;
	USHORT	m_uChaRemain;			// 생성 가능한 캐릭터 수
	BOOL	m_bChaExtreme;			// 극강부 캐릭터 생성 가능한가;
	int		m_nServerGroup;
	int		m_nServerChannel;
	bool	m_bCHANNEL;
	BOOL	m_bLoginCancel;

	LONG	m_lResolutionBack;

	UIGUID	m_uidMoalCallWindow;
	CString	m_strDaumGID;
	CString m_strGeneralInfoBack;
    wb::EM_COUNTRY m_Country; //! 국가

	bool	m_bCharacterSelectLock;

private:
	CModalWindow*				m_pModalWindow;
	CSelectServerPage*			m_pSelectServerPage;
	CLoginPage*					m_pLoginPage;
	CUIControl*					m_pLoginMark;
	CSecPassSetPage*			m_pSecPassSetPage;
	CSecPassCheckPage*			m_pSecPassCheckPage;
	CMessageWindow*				m_pMessageWindow;
	CPassKeyboard*				m_pPassKeyboard;
	CSelectCharacterPage*		m_pSelectCharacterPage;
	CSelectCharacterLeftPage*	m_pSelectCharacterLeftPage;
	CCreateCharacterPage*		m_pCreateCharacterPage;
	CSelectCountryUI*			m_pSelectCountryUI;
	CSelectContinentListUI*		m_pSelectContinentListUI;
	CSelectCountryListUI*		m_pSelectCountryListUI;
	CUIControl*					m_pFullScreenButton;
	CUIControl*					m_pFullScreenButtonDummy;
	CUIControl*					m_pUpImage;
	CUIControl*					m_pDownImage;
	CUIControl*					m_pMessageWindowDummy;
	CNewCreateCharPage*			m_pNewCreateCharPage;
	CCreateCharacterWindow*		m_pCreateCharacterWindow;
	CCreateChatacterButtonPage* m_pCreateCharacterButtonPage;
	CBasicVarTextBox*			m_pInfoDisplay;
	CBasicTextBox*				m_pTitleText;
	CRotateCharacter*			m_pRotateChar;
	
	CWaitDialogue*				m_pWaitDisplay;
	DxLobyStage*				m_pDxLobyStage;
    CBasicTextBox*              m_pCharacterCountryClubInfo;

    CLockSelectCharacterPreviewPage* m_pLockSelectCharacterPreviewPage;
    CSelectCountryConfirmModal* m_pSelectCountryConfirmModal;
    CSelectCountryWindow* m_pSelectCountryWindow;
    CSelectClubWindow* m_pSelectClubWindow;

	void ResetControl();

public:
	COuterInterface(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~COuterInterface();

public:
	virtual UIGUID	GetRenderStage ();

public:
    static void UiSetResolution( long lResolution );
    static long UiGetResolution();

public:
	HRESULT OneTimeSceneInit();
	HRESULT FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
    virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	HRESULT FinalCleanup();
	HRESULT DeleteDeviceObjects();

    virtual HRESULT IOneTimeSceneInit() { return OneTimeSceneInit(); }
    virtual	HRESULT IFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime ) { return FrameMove( pd3dDevice, fElapsedTime ); }
    virtual HRESULT IFinalCleanup() { return FinalCleanup(); }
    virtual HRESULT IDeleteDeviceObjects() { return DeleteDeviceObjects(); }
    virtual HRESULT IInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual HRESULT IInvalidateDeviceObjects();
    virtual HRESULT IRestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual void    IRender( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual BOOL UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep = false );
    virtual BOOL UiIsVisibleGroup(UIGUID ControlID);
    virtual void UiShowGroupTop(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg = false);
    virtual void UiHideGroup(UIGUID ControlID, bool bMakeMsg = false);

public:
	virtual void ToggleUISelectCountry( const bool bToggle );
	virtual void ToggleUIJoinWorldBattle( const bool bToggle );

public:
	void SetOnTheInterface ()							{ m_bOnTheInterface = TRUE; }
	virtual BOOL IsOnTheInterface ()							{ return m_bOnTheInterface; }
	void ResetOnTheInterface ()							{ m_bOnTheInterface = FALSE; }

	void	START_WAIT_TIME ( float fTime );
	float	GET_WAIT_TIME()								{ return m_fWaitTime; }

	void SetCancelToLogin()								{ m_bLoginCancel = TRUE; }
	void ResetCancelToLogin()							{ m_bLoginCancel = FALSE; }
	BOOL IsLoginCancel()								{ return m_bLoginCancel; }

	void SetModalCallWindowID( UIGUID uID )				{ m_uidMoalCallWindow = uID; }
	UIGUID GetModalCallWindowID()						{ return m_uidMoalCallWindow; }
    
    virtual void SetSelectCharIndex( DWORD dwCharSelect, INT nCharStart );

	virtual void SetCharacterSelectLock ( bool bCharacterSelectLock )	{ m_bCharacterSelectLock = bCharacterSelectLock; }
	virtual bool IsCharacterSelectLock ()								{ return m_bCharacterSelectLock; }

private:
	void	UPDATE_WAIT_TIME ( float fElapsedTime );
	void	ModalMsgProcess ( UIGUID nCallerID, DWORD dwMsg );
	void	CloseAllWindow ();

public:
	virtual void MsgProcess ( LPVOID pBuffer );

    //  인터페이스
    virtual void IMsgProcess ( LPVOID pBuffer ) { MsgProcess( pBuffer ); }

	void SetDaumGID( const CString & strDaumGID )	{ m_strDaumGID = strDaumGID; }
	const TCHAR * GetDaumGID() const				{ return m_strDaumGID.GetString(); }

	void SetCharRemain( const USHORT uChaRemain )		{ m_uChaRemain = uChaRemain; }
	const USHORT GetCharRemain() const					{ return m_uChaRemain; }

	void SetCreateExtreme( const bool bCreateExtreme );
	BOOL IsCreateExtreme();

	const UIRECT	GetBackImageRect();

	// 중국 MMOSPEED
	//void RunMMOSpeed(const SUBPATH* pPath);

	const char* MakeString ( const char* szFormat, ... );

	void SetBlockProgramFound ( bool bFOUND )			{ m_bBlockProgramFound = bFOUND; }
	bool IsBlockProgramFound ()							{ return m_bBlockProgramFound; }

	void WAITSERVER_DIALOGUE_OPEN ( const CString& strMessage, const int nAction, const float fTimer );
	void WAITSERVER_DIALOGUE_CLOSE();

	void ToSelectServerPage( UIGUID cID );
	void ToLoginPage( UIGUID cID );
	void ToSecPassPage( UIGUID cID, int nCheckFlag );
	void ToChinaAreaPage( UIGUID cID );
	void ToSelectCharacterPage( UIGUID cID );
	virtual void IToSelectCharacterPage( UIGUID cID ) { ToSelectCharacterPage( cID ); }
	virtual void ToCreateCharacterPage ( UIGUID cID );
    void ToSelectCountryPage();
    void ShowSelectCountryWindow();
    void ShowSelectClubWindow();
    void SendGameJoinMessage();

	void OpenMessageWindow ( UIGUID cID, BOOL bOpen );

	void DeleteCharacter ();
	
	bool IsCHANNEL()									{ return m_bCHANNEL; }

	CModalWindow*			GetModalWindow ()			{ return m_pModalWindow; }
	CSelectServerPage*		GetSelectServerPage ()		{ return m_pSelectServerPage; }
	CLoginPage*				GetLoginPage ()				{ return m_pLoginPage; }
	CSecPassSetPage*		GetSecPassSetPage ()		{ return m_pSecPassSetPage;	}
	CSecPassCheckPage*		GetSecPassCheckPage ()		{ return m_pSecPassCheckPage;	}
	CPassKeyboard*			GetPassKeyboard()			{ return m_pPassKeyboard; }
	CSelectCharacterPage*	GetSelectCharacterPage ()	{ return m_pSelectCharacterPage; }
	CSelectCharacterLeftPage*	GetSelectCharacterLeftPage ()	{ return m_pSelectCharacterLeftPage; } // 2차비번
	CSelectCountryUI*		GetSelectCountryUI()		{ return m_pSelectCountryUI; }
	CSelectContinentListUI* GetSelectContinentListUI()	{ return m_pSelectContinentListUI; }
	CSelectCountryListUI*	GetSelectCountryListUI()	{ return m_pSelectCountryListUI; }
	CCreateCharacterPage*	GetCreateCharacterPage ()	{ return m_pCreateCharacterPage; }
	CNewCreateCharPage*		GetNewCreateCharacterPage (){ return m_pNewCreateCharPage; }
	CCreateCharacterWindow* GetCreateCharacterWindow()	{ return m_pCreateCharacterWindow; }
	CUIControl*				GetUpImage()				{ return m_pUpImage; }
	CUIControl*				GetDownImage()				{ return m_pDownImage; }
	CRotateCharacter*		GetRotateCharacter()		{ return m_pRotateChar; }

	void SetConnectServerInfo ( const int nServerGroup, const int nServerChannel );
	void GetConnectServerInfo ( int& nServerGroup, int& nServerChannel );

	virtual void SetLobyStage( DxLobyStage * pLobyStage )	{ GASSERT( pLobyStage ); m_pDxLobyStage = pLobyStage; }

    virtual void ISetResolution( long lResolution );
	virtual void SetResolutionBack( long lResolution )	{ m_lResolutionBack = lResolution; }
	long GetResolutionBack()						{ return m_lResolutionBack; }

	void SHOW_COMMON_LINEINFO ( const CString& strText, D3DCOLOR dwColor );

	bool BEGIN_COMMON_LINEINFO_MULTI ( int nTextAlignFlag );
	bool ADD_COMMON_LINEINFO_MULTI ( const CString& strText, D3DCOLOR dwColor );
	void END_COMMON_LINEINFO_MULTI ();

    // 로비에서 캐릭터 위에 잠금 이미지 관련.
    virtual void ISetLockImagePos( const D3DXVECTOR3& vCharPos, int nIndex );
    virtual void ISetVisibleLockImage( bool bVisible, int nIndex );

private:
	BOOL LoadClientVersion ( int& nPatchVer, int& nGameVer );

public:
    virtual void DoModal(const std::string& strText, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE, BOOL bPassKey = FALSE, BOOL bTwian = FALSE) override; // 2차비번
	virtual void SetModalData ( int nData1, int nData2 );
	virtual void SetModalData ( D3DXVECTOR3 vData); // by luxes.

public:
//	static COuterInterface& GetInstance();
	virtual void CLOSE_MODAL ( const UIGUID cID, bool bMakeMsg );
    
    //! 국가
    virtual void SetCountry(wb::EM_COUNTRY Country) override;
    wb::EM_COUNTRY GetCountry() const { return m_Country; }

    virtual void ShowCountryClubCountInSelectCountryWindow() override;
    virtual void ReloadClubList() override;

	// 통합전장 로그인 W.B.
	virtual BOOL LoginWorldBattle() override;

    void ShowBeforeSelectModal();
    void ShowAfterSelectModal( const wb::COUNTRY_INFO& CountryInfo );
    void ShowSelectWorldBattleClub();
    void ShowCharacterClubInfo();
    void SetClubListOrder( wb::search::EM_ORDER_TYPE Type, wb::search::EM_ORDER_ASC_DSC Order );
    void SearchClubList();
    void SelectClubList( int nIndex );
};
