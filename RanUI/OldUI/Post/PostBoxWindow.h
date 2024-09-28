#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

#include <vector>

class CInnerInterface;
class CBasicTextButton;
class CBasicLineBox;
class CBasicTextBox;
class CPostBoxWindowObject;
class CPostBoxWindowMain;
class CPostBoxWindowSub;
class CPostBoxWindowNaked;
class CPostBoxPage;
class CPostBoxReceivePage;
class CPostBoxReadPage;
class CPostBoxSendPage;
class CPostBoxHistoryPage;
class CPostBoxFriendFindPage;
class CPostBoxPreviewPage;
class GLGaeaClient;

class CPostBoxWindow : public CUIGroup
{
protected:
	enum
	{
		POSTBOX_NONE = NO_ID,

		POSTBOX_WINDOW_MAIN,
		POSTBOX_WINDOW_SUB,
		POSTBOX_WINDOW_NAKED,

		POSTBOX_PAGE_RECEIVE,
		POSTBOX_PAGE_READ,
		POSTBOX_PAGE_SEND,
		POSTBOX_PAGE_HISTORY,
		POSTBOX_PAGE_FRIENDFIND,
		POSTBOX_PAGE_PREVIEW,
	};

public:
	enum
	{
		POSTBOX_PAGE_REQ_NONE = 0,
		POSTBOX_PAGE_REQ_FRIENDFIND_OPEN,				// 친구 찾기창 열기
		POSTBOX_PAGE_REQ_FRIENDFIND_CLOSE,				// 친구 찾기창 닫기
		POSTBOX_PAGE_REQ_PREVIEW_OPEN,					// 미리 보기창 열기
		POSTBOX_PAGE_REQ_PREVIEW_CLOSE,					// 미리 보기창 닫기
		POSTBOX_PAGE_REQ_FRIENDFIND_SELECT_TO_SEND,		// 친구 찾기창에서 선택 -> 편지 보내기 창 으로
		POSTBOX_PAGE_REQ_READ_OPEN,						// 편지 읽기창 열기
		POSTBOX_PAGE_REQ_READ_CLOSE,					// 편지 읽기창 닫기
		POSTBOX_PAGE_REQ_READ_RETURN,					// 편지 읽기창 반송
		POSTBOX_PAGE_REQ_READ_DELETE,					// 편지 읽기창에서 삭제 요청
		POSTBOX_PAGE_REQ_READ_REPLY,					// 편지 읽기창에서 답장 요청
		POSTBOX_PAGE_REQ_RECEIVE_SELECT_RESET,			// 편지 리스트 선택 리셋
		POSTBOX_PAGE_REQ_RECEIVE_REFRESH,				// 우편리스트 새로고침
	};

	enum
	{
		SLOT_MAIN =  0,
		SLOT_MAX  =  3,
		SLOT_NULL = -1,
	};

public:
	typedef std::vector<CPostBoxWindowObject*>	VECWINDOWSLOT;
	typedef VECWINDOWSLOT::iterator				VECWINDOWSLOTITER;

	typedef std::vector<CPostBoxPage*>			VECPAGE;
	typedef VECPAGE::iterator					VECPAGEITER;

public:
	CPostBoxWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxWindow();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual HRESULT		 InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void		 Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		 TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void		 SetVisibleSingle( BOOL bVisible );

public:
	void				 CreateSubControl( LPDIRECT3DDEVICEQ pd3dDevice );

public :
	void				 Reset( const BOOL bUpdate=FALSE );

public:
	void				 Close();

public:
	void				 WindowResize();

public:
	void				 WindowBegin();
	void				 WindowEnd();

public:
	const BOOL			 IsWindowOpenAble( const BOOL bPrint=FALSE );
	const BOOL			 IsWindowCloseAble( const BOOL bPrint=FALSE );

public:
	const BOOL			 IsPageOpen( CPostBoxPage* pPage );

public:
	void				 WindowPageOpen( UIGUID WndControlID, CPostBoxPage* pPage );
	void				 WindowPageOpen( CPostBoxWindowObject* pWindow, CPostBoxPage* pPage );
	void				 WindowPageOpenSlot( CPostBoxWindowObject* pWindow, CPostBoxPage* pPage, const DWORD dwSlot );

public:
	void				 WindowPageClose( UIGUID WndControlID);
	void				 WindowPageClose( CPostBoxWindowObject* pWindow );
	void				 WindowPageClose( CPostBoxPage* pPage );

private:
	void				 UpdatePageRequest( UIGUID ControlID, const DWORD dwRequest );

private:
	CBasicTextBox*		 CreateStaticControl(const char* szConatrolKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID = NO_ID );
	CBasicLineBox*		 CreateBackgroundControl( char* szConatrolKeyword );

public:
	const DWORD			 GetEnableSlotCount();
	const DWORD			 GetEmptySlot();

public:
	CPostBoxSendPage*	 GetSendPage()	{ return m_pPage_Send;		}
	CPostBoxReceivePage* GetReceivePage()	{ return m_pPage_Receive;	}
	CPostBoxReadPage*	 GetReadPage()	{ return m_pPage_Read;		}
	CPostBoxHistoryPage* GetHistoryPage()	{ return m_pPage_History;	}
	CPostBoxPreviewPage* GetPreviewPage()	{ return m_pPage_Preview;	}
	
public:
	void Process_ReceiveComplete(); // Note : 우편받기 완료 프로세스
    void PostBoxHistoryPageUpdateList();
    void RequestPushViewPage( const DWORD dwRequest );
    void ReqMultiDelete_Check();
    void ReqMultiDelete_Commit();
    void PostBoxReceivePageUpdateItem();
    void ReqSendPost();
    void ResetSendPage();
    bool IsSendPageEnalbe();
    const BOOL IsSendPageOpen();
    void Set_TO_Editbox( const UI::String& strText_To );
    void ReqPostDelete( bool bConfirmAttach );
    void ReqPostReturn();
    void ReqPostTakeItem();

public:
	void DoModalError( const std::string& strError);
	void DoModalMsg( const std::string& strMsg);

private:
	void RefreshMainPage();
	//void	SetVariableTextControl ();

private:
	CPostBoxWindowMain*		m_pWindow_Main;
	CPostBoxWindowSub*		m_pWindow_Sub;
	CPostBoxWindowNaked*	m_pWindow_Naked;

private:
	CPostBoxReceivePage*	m_pPage_Receive;
	CPostBoxReadPage*		m_pPage_Read;
	CPostBoxSendPage*		m_pPage_Send;
	CPostBoxHistoryPage*	m_pPage_History;
	CPostBoxFriendFindPage*	m_pPage_FriendFind;
	CPostBoxPreviewPage*	m_pPage_Preview;

private:
	LPDIRECT3DDEVICEQ		m_pd3dDevice;

private:
	BOOL					m_bOpen;
	VECWINDOWSLOT			m_vecSlot;
	VECPAGE					m_vecPage;

protected:
	CInnerInterface* m_pInterface;

};

class MyPostBoxWindow : public IPostBoxWindow, private CPostBoxWindow
{
public:
    MyPostBoxWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyPostBoxWindow()  {};

public:
    virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual void Close();
    virtual void DoModalError( const std::string& strError );
    virtual void DoModalMsg( const std::string& strMsg);
    virtual CPostBoxHistoryPage* GetHistoryPage();
    virtual CPostBoxPreviewPage* GetPreviewPage();
    virtual CPostBoxReceivePage* GetReceivePage();
    virtual CPostBoxSendPage* GetSendPage();
    virtual BOOL IsVisible();
    virtual const BOOL IsWindowOpenAble( const BOOL bPrint=FALSE );
    virtual const BOOL IsWindowCloseAble( const BOOL bPrint=FALSE );
    virtual void Process_ReceiveComplete();
    virtual CPostBoxReadPage* GetReadPage();
    virtual const BOOL IsSendPageOpen();
    virtual void PostBoxHistoryPageUpdateList();
    virtual void RequestPushViewPage( const DWORD dwRequest );
    virtual void ReqMultiDelete_Check();
    virtual void ReqMultiDelete_Commit();
    virtual void PostBoxReceivePageUpdateItem();
    virtual void ReqSendPost();
    virtual void ResetSendPage();
    virtual bool IsSendPageEnalbe();
    virtual void Set_TO_Editbox( const UI::String& strText_To );
    virtual void ReqPostDelete( bool bConfirmAttach = true );
    virtual void ReqPostReturn();
    virtual void ReqPostTakeItem();
};
