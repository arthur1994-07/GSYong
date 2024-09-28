#pragma once

#include "DxVmrAllocator.h"

namespace NSUSERID
{
	extern DWORD_PTR g_pUserId;
}

class DShowPlayer
{
private :
	DShowPlayer(void);
	~DShowPlayer(void);

public :
	static DShowPlayer& GetInstance();

private :
	enum
	{
		MIN_DECIBEL = -10000,
		MAX_DECIBEL = 0,
	};

private :
	LPDIRECT3DDEVICE9	m_pDevice;

	HWND m_hWnd;
	long m_lPlayEndMsg;

	CComQIPtr<IGraphBuilder>			m_pGraph;
	CComQIPtr<IVMRFilterConfig9>		m_pConfig;
	CComQIPtr<IVMRSurfaceAllocator9>	m_pAlloc;
	CComQIPtr<IMediaEventEx>			m_pMediaEvent;
	CComQIPtr<IMediaControl>			m_pMediaCtrl;
	CComQIPtr<IMediaPosition>			m_pMediaPos;
	CComQIPtr<IBasicAudio>				m_pAudio;

	DxVmrAllocator*						m_pVmrAlloc;

private :
	std::string m_strPathFile;
	REFTIME		 m_llLostTime;
	bool		 m_bPlayingVideo;
	int			 m_nVolume;

public :
	void Release();

public :
	HRESULT Init( HWND hWnd, long lMsg, LPDIRECT3DDEVICE9 pDevice );
	HRESULT Load( const std::string& strFile );
	void OnResetDevice();
	void OnLostDevice();

public :
	void Play();
	void Stop();
	bool IsComplete();

	bool IsPlayingVideo() { return m_bPlayingVideo; }

	void SetVolume( WORD wVolume, int nControlVolume );
};
