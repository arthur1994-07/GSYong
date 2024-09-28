#pragma once

#include <math.h>

#include "./DShowMemReader.h"

struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

class DShowTextureRenderer : public CBaseVideoRenderer
{
public :
	DShowTextureRenderer( LPUNKNOWN pUnk, HRESULT *phr, LPDIRECT3DDEVICE9 pDevice, 
		LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget );
	~DShowTextureRenderer();

private :
	LPDIRECT3DDEVICE9 m_pDevice;

	LPDIRECT3DTEXTURE9* m_ppTexture;
	LPDIRECT3DSURFACE9* m_ppRenderTarget;

public :
	BOOL m_bUseDynamicTextures;
	LONG m_lVidWidth;   // Video width
	LONG m_lVidHeight;  // Video Height
	LONG m_lVidPitch;   // Video Pitch

public :
	HRESULT ResetTexture();

public :
	virtual HRESULT CheckMediaType( const CMediaType* pMediaType );
	virtual HRESULT SetMediaType( const CMediaType* pMediaType );
	virtual HRESULT DoRenderSample( IMediaSample* pMediaSample );
};

//-----------------------------------------------------------------------

class DShowTexture
{
public :
	DShowTexture(void);
	virtual ~DShowTexture(void);

private :
	enum
	{
		MIN_DECIBEL = -10000,
		MAX_DECIBEL = 0,

		LEFT_BALANCE = -10000,
		RIGHT_BALANCE = 10000,
	};

private :
	CComQIPtr< IGraphBuilder >	m_pGraph;
	CComQIPtr< IFilterGraph >	m_pFilterGraph;
	CComQIPtr< IMediaControl >	m_pMediaCtrl;
	CComQIPtr< IMediaPosition >	m_pMediaPos;
	CComQIPtr< IMediaEvent >	m_pMediaEvent;
	CComQIPtr< IBaseFilter >	m_pRenderer;
	CComQIPtr< IBasicAudio >	m_pAudio;

private :
	DShowTextureRenderer*	m_pTextureRenderer;
	DShowMemReader*			m_pMemReader;
	DShowMemStream			m_Stream;

public :
	void OnResetDevice();

public :
	HRESULT InitTextureRenderer( LPDIRECT3DDEVICE9 pDevice, const std::string& strFile, 
		LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget );
	HRESULT InitTextureRendererFromBuffer( LPDIRECT3DDEVICE9 pDevice, const std::string& strFile, 
		LPDIRECT3DTEXTURE9* ppTexture, LPDIRECT3DSURFACE9* ppRenderTarget,
		LPBYTE pBuffer, LONGLONG llLength );

	void CheckVideoStatus();

public :
	void PlayVideo();
	void PauseVideo();
	void StopVideo();

	void SetVolume( WORD wVolume, int nControlVolume );
	void Set3DVolume( const D3DXVECTOR3& vPosition, 
		const D3DXVECTOR3& vViewPos, const D3DXVECTOR3& vViewRight, float fRangeSq, int nControlVolume );
};
