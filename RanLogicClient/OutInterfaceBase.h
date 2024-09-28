#ifndef _OUT_INTERFACE_BASE_H_
#define _OUT_INTERFACE_BASE_H_

#pragma once

#include "../EngineLib/GUInterface/UIDataType.h"
#include "../EngineLib/GUInterface/InterfaceBaseDefine.h"

class DxLobyStage;
class GLGaeaClient;
class EngineDeviceMan;

class OutInterfaceBase
{
public:
    OutInterfaceBase(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
    virtual ~OutInterfaceBase();

protected:
    GLGaeaClient* m_pGaeaClient;
    EngineDeviceMan* m_pEngineDevice;

public:
    GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }
    EngineDeviceMan* GetEngineDevice() { return m_pEngineDevice; }

public: 
    virtual UIGUID	GetRenderStage () = 0;
    virtual BOOL IsOnTheInterface () = 0;
    virtual void SetSelectCharIndex( DWORD dwCharSelect, INT nCharStart ) = 0;
    virtual void ToCreateCharacterPage ( UIGUID cID ) = 0;
    virtual void SetLobyStage( DxLobyStage * pLobyStage ) = 0;
    
    virtual void ISetResolution( long lResolution ) = 0;
    virtual void SetResolutionBack( long lResolution ) = 0;
  
    virtual void IMsgProcess( LPVOID pBuffer ) = 0;

    virtual HRESULT IOneTimeSceneInit() = 0;
    virtual	HRESULT IFrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime ) = 0;
    virtual HRESULT IFinalCleanup() = 0;
    virtual HRESULT IDeleteDeviceObjects() = 0;
    virtual HRESULT IInitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual HRESULT IInvalidateDeviceObjects() = 0;
    virtual HRESULT IRestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;
    virtual void    IRender( LPDIRECT3DDEVICEQ pd3dDevice ) = 0;

    // 로비에서 캐릭터 위에 잠금 이미지 관련.
    virtual void ISetLockImagePos( const D3DXVECTOR3& vCharPos, int nIndex ) = 0;
    virtual void ISetVisibleLockImage( bool bVisible, int nIndex ) = 0;

    virtual void SetCountry(wb::EM_COUNTRY Country) = 0;
    virtual void ShowCountryClubCountInSelectCountryWindow() = 0;
    virtual void ReloadClubList() = 0;

	virtual BOOL LoginWorldBattle() = 0;

    virtual void DoModal(const std::string& strText, int nModalTitle = UI::MODAL_NOTITLE, int nModalType = UI::OK, UIGUID CallerID = NO_ID, BOOL bHide = FALSE, BOOL bPassKey = FALSE, BOOL bTwian = FALSE ) = 0;

	// UI Toggle;
	virtual void ToggleUISelectCountry( const bool bToggle ) = 0;

	virtual void ToggleUIJoinWorldBattle( const bool bToggle ) = 0;

	// 임시 추가
	virtual void IToSelectCharacterPage( UIGUID cID ) = 0;

	virtual void SetCharacterSelectLock ( bool bCharacterSelectLock ) = 0;
	virtual bool IsCharacterSelectLock () = 0;
};


#endif // _OUT_INTERFACE_BASE_H_
