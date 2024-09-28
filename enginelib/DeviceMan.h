#ifndef _ENGINE_DEVICE_MAN_H_
#define _ENGINE_DEVICE_MAN_H_

#pragma once

#include "../InternalCommonLib/dxstdafx.h"
#include "./ServiceProviderDefine.h"
#include "./DxTools/DxFontDefine.h"

class DXInputString;
class CD3DFontPar;

class EngineDeviceMan
{
public:
    EngineDeviceMan(EMSERVICE_PROVIDER emServiceProvider);
    virtual ~EngineDeviceMan();

protected:
    EMSERVICE_PROVIDER  m_emServiceProvider;

public:
    inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }

public:
    virtual LPDIRECT3DDEVICEQ GetDevice() = 0;
    virtual DXInputString* GetInputString() = 0;
    virtual HRESULT SetRenderState() = 0;
    virtual UINT GetBackBufferWidth() = 0;
    virtual UINT GetBackBufferHeight() = 0;
	virtual void DoInterimClean_Prev() = 0;
    virtual HRESULT DoInterimClean(LPDIRECT3DDEVICEQ pd3dDevice) = 0;
    virtual HRESULT OneTimeSceneInit(
        const char* szPath,
        CWnd* pWndApp,
        const CString& strFontSys,
        bool bGameMode,
        language::LANGFLAG dwLang=language::DEFAULT,
        const CString& strFontName=_T("Tahoma")) = 0;

    virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCAPTUREMOUSE=FALSE) = 0;
    virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
    virtual HRESULT DeleteDeviceObjects() = 0;
    virtual HRESULT FinalCleanup() = 0;
    virtual HRESULT FrameMove(float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame=FALSE) = 0;
	virtual HRESULT FrameMoveFirst ( float fTime, float fElapsedTime ) = 0;
	virtual HRESULT FrameMoveSecond ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame ) = 0;
    //HRESULT Render ();
    virtual LRESULT MsgProc(MSG* pMsg) = 0;

    virtual CD3DFontPar* LoadFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags=0L) = 0;
    virtual CD3DFontPar* FindFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags) = 0;
};

#endif // _ENGINE_DEVICE_MAN_H_