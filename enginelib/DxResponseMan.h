#ifndef ENGINE_DX_RESPONSE_MAN_H_
#define ENGINE_DX_RESPONSE_MAN_H_

#pragma once

#include "./DeviceMan.h"

class SUBPATH;

// 여러가지 장치 Class 들을 생성, 해제를 모아서 처리하기 위해서 사용함.
class DxResponseMan : public EngineDeviceMan
{
public:
	DxResponseMan(EMSERVICE_PROVIDER emServiceProvider, SUBPATH* pPath);
	virtual ~DxResponseMan();

protected:
	char m_szFullPath[MAX_PATH];

	CWnd*				m_pWndApp;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	//LPDIRECT3DQ			m_pD3D;
	D3DCAPSQ			m_d3dCaps;
	D3DSURFACE_DESC		m_d3dsdBackBuffer;
	DXInputString*      m_pInputString;
	SUBPATH*            m_pPath;
    bool                m_bGameMode;

public:
	virtual void DoInterimClean_Prev() override;
    virtual HRESULT DoInterimClean(LPDIRECT3DDEVICEQ pd3dDevice) override;

    virtual HRESULT OneTimeSceneInit(
		const char* szPath,
		CWnd* pWndApp,
		const CString& strFontSys,
        bool bGameMode,
		language::LANGFLAG dwLang = language::DEFAULT,
		const CString& strFontName = _T("Tahoma") ) override;

	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCAPTUREMOUSE=FALSE) override;
	virtual HRESULT RestoreDeviceObjects() override;
	virtual HRESULT InvalidateDeviceObjects() override;
	virtual HRESULT DeleteDeviceObjects() override;
	virtual HRESULT FinalCleanup() override;
	virtual HRESULT FrameMove(float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame=FALSE) override;
	virtual HRESULT FrameMoveFirst ( float fTime, float fElapsedTime ) override;
	virtual HRESULT FrameMoveSecond ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame ) override;
	virtual LRESULT MsgProc(MSG* pMsg) override;

    virtual HRESULT SetRenderState() override;

	void InitTest(HWND hWnd,LPDIRECT3DDEVICEQ pd3dDevice);

public:
    virtual LPDIRECT3DDEVICEQ GetDevice() override { return m_pd3dDevice; }
	virtual DXInputString* GetInputString() override { return m_pInputString; }
	virtual UINT GetBackBufferWidth() override { return m_d3dsdBackBuffer.Width; }
	virtual UINT GetBackBufferHeight() override { return m_d3dsdBackBuffer.Height; }

    virtual CD3DFontPar* LoadFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags=0L) override;
    virtual CD3DFontPar* FindFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags) override;
};

#endif // ENGINE_DX_RESPONSE_MAN_H_