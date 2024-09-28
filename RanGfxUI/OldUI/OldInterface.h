#pragma once

#include "../../enginelib/GUInterface/UIControl.h"
#include "../../enginelib/GUInterface/UIManCommon.h"
#include "../../RanLogicClient/Crow/GLCrowRenList.h"
#include "../../enginelib/GUInterface/UIMan.h"

class GLGaeaClient;
class EngineDeviceMan;
class SUBPATH;
class CSimpleHP;

class CDamageDisplayMan;
class CNameDisplayMan;
class CPrivateMarketShowMan;
class CHeadChatDisplayMan;
class CTargetInfoDisplay;
class CCursorTargetInfo;

class OldInterface : public ITranslateUIMsg
{
public :
	OldInterface( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice );
	virtual ~OldInterface();

private :
	GLGaeaClient* m_pGaeaClient;
	EngineDeviceMan* m_pEngineDevice;
	SUBPATH* m_pPath;

public :
	GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }
	SUBPATH* GetSubPath() { return m_pPath; }

private :
	CDamageDisplayMan* m_pDamageDisplayMan;
	CSimpleHP*					m_pSimpleHP;				
	CNameDisplayMan* m_pNameDisplayMan;
	CPrivateMarketShowMan* m_pPrivateMarketShowMan;
	CHeadChatDisplayMan* m_pHeadChatDisplayMan;
	CTargetInfoDisplay*	m_pTargetInfoDisplay;
	CCursorTargetInfo* m_pCursorTargetInfo;

public :
	static void UiSetResolution( long lResolution );
	static long UiGetResolution();

public :
	HRESULT UiFinalCleanup();
	HRESULT UiFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	HRESULT UiRender( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UiInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UiRestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UiInvalidateDeviceObjects();
	HRESULT UiDeleteDeviceObjects();

	BOOL UiRegisterControl( CUIControl* pNewControl, bool bPosisionKeep = false );
	BOOL UiIsVisibleGroup(UIGUID ControlID);
	void UiShowGroupUpdate(UIGUID ControlID, bool bMakeMsg = false);
	void UiShowGroupTop(UIGUID ControlID, bool bMakeMsg = false);
	void UiShowGroupBottom(UIGUID ControlID, bool bMakeMsg = false);
	void UiShowGroupFocus(UIGUID ControlID, bool bMakeMsg = false);
	void UiSetGroupLock(UIGUID ControlID, bool bLock = true);
	void UISetModalGroup(UIGUID ControlID, bool bModal = true);
	void UiHideGroup(UIGUID ControlID, bool bMakeMsg = false);
	void UiRefresh( UIGUID ControlID );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects();    
	HRESULT FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime );
	HRESULT FinalCleanup();

private :
	void ResetControl();

public :
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	void SetDamage( D3DXVECTOR3 vPos, int nDamage, DWORD dwDamageFlag, BOOL bAttack );

	void NameDisplaySetCount( DWORD Count );
	void NameDisplayAddName( CROWREN &sDISP_NAME, const char * szOwnerName = NULL );
	void NameDisplayShowCountryMark( bool bShowCountry );
	void ClearNameList ();

	void	ADD_MARKET( const CROWREN &sDISP );
	void	SetPrivateMarketMake ();
	void	SetPrivateMarketOpen ( const bool& bOPENER, const DWORD& dwGaeaID );
	void	GetPrivateMarketInfo ( bool& bOPENER, DWORD& dwGaeaID );
	void	SetPrivateMarketClose ();	
	CPrivateMarketShowMan* GetPrivateMarketShowNam() { return m_pPrivateMarketShowMan; }

	void ResetTargetInfoMouseOver();
	void ResetTargetInfoSelect();
	void SetTargetInfoMouseOver ( STARGETID sTargetID );
	void SetTargetInfoSelect ( STARGETID sTargetID );
	void SetTargetInfo( STARGETID sTargetID, const UI::EMTARGETINFO_TYPE emTargetInfoType );
	
	void AddChatMsg( const CString& strName, const D3DCOLOR& dwIDColor, const CString& strChat, const D3DCOLOR& dwChatColor, const D3DXVECTOR2& vPos );

	void UpdateStateSimpleHP ();
};
