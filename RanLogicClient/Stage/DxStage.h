#ifndef DXSTAGE_H_
#define DXSTAGE_H_

#pragma once

#include "../../InternalCommonLib/dxstdafx.h"

class DxStage
{
public:
    DxStage()
        : m_hWnd(NULL)
        , m_pd3dDevice(NULL)
		, m_bWB( false )
      {
      }

      virtual ~DxStage()
      {
      }

protected:
	char				m_szAppPath[MAX_PATH];
	HWND				m_hWnd;
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	bool				m_bWB;

public:
	virtual HRESULT SetActive ( LPDIRECT3DDEVICEQ pd3dDevice, HWND hWnd, WORD wWidth, WORD wHeight, bool bWBStage = false )
	{
		HRESULT hr;

		if( bWBStage == false )
		{
			hr = OneTimeSceneInit ( hWnd, wWidth, wHeight, m_szAppPath );
			if (FAILED(hr) )	return hr;

			hr = InitDeviceObjects ( pd3dDevice );
			if (FAILED(hr) )	return hr;

			hr = RestoreDeviceObjects ();
			if (FAILED(hr) )	return hr;
		}

		m_bWB = bWBStage;

		return S_OK;
	}

	virtual HRESULT DeActive ()
	{
		HRESULT hr;

		if( m_bWB == false )
		{
			hr = InvalidateDeviceObjects ();
			if (FAILED(hr) )	return hr;

			hr = DeleteDeviceObjects ();
			if (FAILED(hr) )	return hr;

			hr = FinalCleanup ();
			if (FAILED(hr) )	return hr;
		}

		m_bWB = false;

		return S_OK;
	}

	virtual bool IsWBStage() { return m_bWB; }

public:
	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )	{ return S_OK; }
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects ()								{ return S_OK; }
	virtual HRESULT InvalidateDeviceObjects ()							{ return S_OK; }
	virtual HRESULT DeleteDeviceObjects ()								{ return S_OK; }
	virtual HRESULT FinalCleanup ()										{ return S_OK; }

	virtual HRESULT Test()												{ return S_OK; }
	virtual HRESULT Test2()												{ return S_OK; }

public:
	virtual HRESULT FrameMove ( float m_fTime, float m_fElapsedTime, BOOL bNotRendering )	{ return S_OK; }
	virtual HRESULT Render ()											{ return S_OK; }

	virtual BOOL	GetUsedMaterialSystem()	= 0;

	virtual void MsgProcess ( NET_MSG_GENERIC* nmg ) = 0;
	virtual HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight )			{ return S_OK; }
};

#endif // DXSTAGE_H_