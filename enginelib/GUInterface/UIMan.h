#pragma	once

#include "../../SigmaCore/gassert.h"

#include "UIControlContainer.h"
#include "UIFocusContainer.h"
#include "UIMessageQueue.h"
#include "UIDockingMan.h"
#include "PositionKeeper.h"
#include "InterfaceBaseDefine.h"

class CUIControl;

class ITranslateUIMsg;

class CUIMan
{
	static CString m_strPath; // GUI �н�
	static long	m_lResolution; // MEMO : �ػ�

	CPositionKeeper		m_PositionKeeper;
	CUIControlContainer	m_ControlContainer;
    CUIDockingMan       m_DockingMan;
	//CUIMessageQueue	m_ActionMsgQ;
	CUIFocusContainer	m_BottomList;
	CUIFocusContainer	m_FocusList;
	CUIFocusContainer	m_TopList;
	CUIFocusContainer	m_UpdateList;	// �׻� ������Ʈ�� �ʿ��Ѱ͵� �ִ�.

public:
	static BOOL m_bBIGFONT;

    static bool m_bFirstControl;

private:
	//BOOL m_bFirstControl;
	BOOL m_bMouseInControl;

	BOOL m_bExclusiveControl;			// MEMO : ���� ��Ʈ���� ���� ����
	CUIControl * m_pExclusiveControl;	// MEMO : ���� ��Ʈ���� ��Ʈ�� ������

	//BOOL m_bFocusControl;				// MEMO : ��Ŀ�� ��Ʈ���� ���� ����
	CUIControl * m_pFocusControl;		// MEMO : ��Ŀ�� ��Ʈ���� ��Ʈ�� ������

private:
	LPDIRECT3DDEVICEQ m_pd3dDevice;

public:
	CUIMan ();
	virtual	~CUIMan ();

public:
	static const CString & GetPath() 			{ return m_strPath; }
	static void SetPath( const char * szPath )	{ GASSERT( szPath ); m_strPath = szPath; }

    static void SetResolution( long lResolution )   { m_lResolution = lResolution; }
	static long GetResolution()						{ return m_lResolution; }

public:
	CUIControl * GetFocusControl();

    LPDIRECT3DDEVICEQ GetDevice() { return m_pd3dDevice; }

public:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT InvalidateDeviceObjects();
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT DeleteDeviceObjects();		
    HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
    HRESULT FrameMove( float fElapsedTime, ITranslateUIMsg* pInterface );
    HRESULT FinalCleanup();

public:
	void SetMouseInControl( BOOL bMouseInControl )	{ m_bMouseInControl = bMouseInControl; }
	BOOL IsMouseInControl()							{ return m_bMouseInControl; }

	BOOL IsExclusiveControl()						{ return m_bExclusiveControl; }
	void ResetExclusiveControl();

	CUIControl* GetExactFocusControl ();
	void SetExactFocusControl (CUIControl* FocusControl);
	void ResetExactFocusControl();

private:
	BOOL UpdateList( CUIFocusContainer* pList, BOOL bFocusList, float fElapsedTime, ITranslateUIMsg* pInterface );
	HRESULT	RenderList( CUIFocusContainer* pList, LPDIRECT3DDEVICEQ pd3dDevice );
	
public:
	CUIControlContainer* GetControlContainer()	{ return &m_ControlContainer; }
	CUIFocusContainer* GetBottomList()			{ return &m_BottomList; }
	CUIFocusContainer* GetFocusList()			{ return &m_FocusList; }
	CUIFocusContainer* GetTopList()				{ return &m_TopList; }
	CUIFocusContainer* GetUpdateList()			{ return &m_UpdateList; }

	CPositionKeeper* GetPositionKeeper()		{ return &m_PositionKeeper; }

public:
	void ShowGroupBottom( UIGUID ControlID, bool bMakeMsg = false );
	void ShowGroupFocus( UIGUID ControlID, bool bMakeMsg = false  );
	void ShowGroupTop( UIGUID ControlID, bool bMakeMsg = false  );
	void ShowGroupUpdate( UIGUID ControlID, bool bMakeMsg = false  );
	void HideGroup( UIGUID ControlID, bool bMakeMsg = false  );	
	void Refresh( UIGUID ControlID );

	BOOL IsVisibleGroup(UIGUID ControlID);

	/**
		Ư�� ��Ʈ�� �׷쿡 ��Ŀ��,���콺�̺�Ʈ�� �����ϰ� ���� ���� �Ѵ�.
		�̸� �����ϸ� ������ ��Ʈ�� �׷��� Ȱ��ȭ �� �� Ư�� �׷� ��Ʈ�ѵ��� ��Ȱ��ȭ ��Ű��
		Ư�� ��Ʈ�� �׷쿡 ���� ���ó�� �۵� �ǰ� �� �� �ִ�.
	 */
	void SetGroupLock(UIGUID ControlID, bool bLock = true);
	bool IsGroupLock(UIGUID ControlID);

    /**
        Ư�� ��Ʈ�� �׷��� ���ȭ ó�� �Ѵ�.        
     */
    void SetModalGroup(UIGUID ControlID, bool bModal = true);

public:
    BOOL RegisterControl ( CUIControl* pNewControl, bool bPosisionKeep = false );
    BOOL RegisterDockingControl ( CUIControl* pNewControl, bool bParent = false );
	CUIControl*	FindControl ( UIGUID ControlID );
	BOOL DeleteControl ( UIGUID ControlID );

public:
	void DeleteAllControl ();

public:
    void MOVE_DEFAULT_POSITION ();

//public:
//	void PostUIMessage ( UIGUID cID, DWORD dwMsg );
//
//public:
//	void UpdateActionMsgQ ();	
//	void TranslateActionMsgQ ( UIGUID ControlID, DWORD dwMsg );

private:
	void SetControlPosition();
	void MaintainControlPosition();
};