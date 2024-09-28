#include "stdafx.h"
#include "./NSDShow.h"

#include "./DShowPlayer.h"
#include "./DShowTexture.h"

#define DSHOW_SAFE_DELETE( p ) { \
	if( NULL != p )\
	{\
		delete p;\
		p = NULL;\
	} }

#define DSHOW_SAFE_RELEASE( p ) { \
	if( NULL != p )\
	{\
		p->Release();\
		p = NULL;\
	} }

namespace NSDSHOW
{
	std::string g_strVideoPath;

	struct DeleteMapObject
	{
		template< typename KEY, typename VALUE >
		void operator () ( std::pair< KEY, VALUE > MapPair )
		{
			DSHOW_SAFE_DELETE( MapPair.second );
		}
	};
};

namespace NSDSHOW_SOUND
{
	int g_nCONTROL_DECIBEL = 5000;
};

// --------------------------------------------------------------
// Player - ȭ�� ��ü�� ���� ���
namespace NSDSHOW
{
	

	// Player �ʱ�ȭ ( long lMsg - ��� �Ϸ� �޽��� )
	void InitPlayer( HWND hWnd, long lMsg, LPDIRECT3DDEVICE9 pDevice )
	{
		DShowPlayer::GetInstance().Init( hWnd, lMsg, pDevice );
	}

	// �ε� & ���
	HRESULT LoadAndPlay( const char* szFile )
	{
		std::string strPathFile = g_strVideoPath;
		strPathFile.append( szFile );

		if( SUCCEEDED( DShowPlayer::GetInstance().Load( strPathFile ) ) )
		{
			DShowPlayer::GetInstance().Play();
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}

	// ��� �Ϸ� �Ǿ����� Ȯ�� ( �Ϸ� �ƴٸ� DXUTPause ���� )
	bool IsPlayerPlayEnd()
	{
		if( DShowPlayer::GetInstance().IsComplete() )
		{
			ReleasePlayer();
			return true;
		}
		else
		{
			return false;
		}
	}

	void ReleasePlayer()
	{
		DShowPlayer::GetInstance().Release();
	}

	void OnResetDevicePlayer()
	{
		DShowPlayer::GetInstance().OnResetDevice();
	}

	void OnLostDevicePlayer()
	{
		DShowPlayer::GetInstance().OnLostDevice();
	}

	// ���� ����
	void SetPlayerVolume( WORD wVolume )
	{
		DShowPlayer::GetInstance().SetVolume( wVolume, NSDSHOW_SOUND::g_nCONTROL_DECIBEL );
	}
};


// --------------------------------------------------------------
// Texture - �ؽ�ó�� ���� ������
namespace NSDSHOW
{
	int g_nCONTROL_DECIBEL = 5000;

	SDshowTexture::SDshowTexture()
		: pTexture( NULL )
		, pSurface( NULL )
	{
		pRenderer = new DShowTexture;
	}

	SDshowTexture::~SDshowTexture()
	{
		DSHOW_SAFE_DELETE( pRenderer );
		DSHOW_SAFE_RELEASE( pTexture );
		DSHOW_SAFE_RELEASE( pSurface );
	}
	// -----------------------------------------

	DShowTextureMan::DShowTextureMan(void)
		: m_pDevice( NULL )
	{
		CoInitializeEx( NULL, COINIT_MULTITHREADED );
	}

	DShowTextureMan::~DShowTextureMan(void)
	{
		OnDestroyDevice();
		CoUninitialize();
	}

	DShowTextureMan::DShowTextureMan( const DShowTextureMan& value )
	{
	}

	DShowTextureMan& DShowTextureMan::GetInstance()
	{
		static DShowTextureMan Instance;
		return Instance;
	}

	void DShowTextureMan::OnCreateDevice( LPDIRECT3DDEVICE9 pDevice )
	{
		m_pDevice = pDevice;
	}

	void DShowTextureMan::OnResetDevice()
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.begin();
		for( ; iter != m_mapVideoTexture.end(); ++iter )
			iter->second->pRenderer->OnResetDevice();
	}

	void DShowTextureMan::OnLosetDevice()
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.begin();
		for( ; iter != m_mapVideoTexture.end(); ++iter )
		{
			DSHOW_SAFE_RELEASE( iter->second->pTexture );
			DSHOW_SAFE_RELEASE( iter->second->pSurface );
		}
	}

	void DShowTextureMan::OnDestroyDevice()
	{
		std::for_each( m_mapVideoTexture.begin(), m_mapVideoTexture.end(), DeleteMapObject() );
		m_mapVideoTexture.clear();
	}

	// E_POINTER : ����̽� ������ ����.
	// S_FALSE : �̹� �����Ǿ� ����.
	// E_FAIL : ���� ����
	HRESULT DShowTextureMan::CreateVideoTexture( const char* szFile, LPDIRECT3DTEXTURE9* ppTexture )
	{
		if( NULL == m_pDevice )
			return E_POINTER;

		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter != m_mapVideoTexture.end() )
		{
			ppTexture = &(iter->second->pTexture);
			return S_FALSE;
		}

		std::string strPathFile = g_strVideoPath;
		strPathFile.append( szFile );

		SDshowTexture* pVideoTexture = new SDshowTexture;
		if( FAILED( pVideoTexture->pRenderer->InitTextureRenderer( m_pDevice, strPathFile, 
			&pVideoTexture->pTexture, &pVideoTexture->pSurface ) ) )
		{
			DSHOW_SAFE_DELETE( pVideoTexture );
			return E_FAIL;
		}

		m_mapVideoTexture.insert( std::make_pair( szFile, pVideoTexture ) );

		ppTexture = &pVideoTexture->pTexture;

		return S_OK;
	}

	HRESULT DShowTextureMan::CreateVideoTexture( const char* szFile, LPBYTE pBuffer, LONGLONG llLength, 
		LPDIRECT3DTEXTURE9* ppTexture )
	{
		if( NULL == m_pDevice )
			return E_POINTER;

		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter != m_mapVideoTexture.end() )
		{
			ppTexture = &(iter->second->pTexture);
			return S_FALSE;
		}

		SDshowTexture* pVideoTexture = new SDshowTexture;
		if( FAILED( pVideoTexture->pRenderer->InitTextureRendererFromBuffer( m_pDevice, szFile, 
			&pVideoTexture->pTexture, &pVideoTexture->pSurface, pBuffer, llLength ) ) )
		{
			DSHOW_SAFE_DELETE( pVideoTexture );
			return E_FAIL;
		}

		m_mapVideoTexture.insert( std::make_pair( szFile, pVideoTexture ) );

		ppTexture = &pVideoTexture->pTexture;

		return S_OK;
	}

	void DShowTextureMan::ReplayVideo( const char* szFile )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->CheckVideoStatus();
	}

	void DShowTextureMan::PlayVideo( const char* szFile )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->PlayVideo();
	}

	void DShowTextureMan::PauseVideo( const char* szFile )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->PauseVideo();
	}

	void DShowTextureMan::StopVideo( const char* szFile )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->StopVideo();
	}

	void DShowTextureMan::StopAllVideo()
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.begin();
		for( ; iter != m_mapVideoTexture.end(); ++iter )
			iter->second->pRenderer->StopVideo();
	}

	void DShowTextureMan::SetVolume( const char* szFile, WORD wVolume )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->SetVolume( wVolume, NSDSHOW_SOUND::g_nCONTROL_DECIBEL );
	}

	void DShowTextureMan::Set3DVolume( const char* szFile, const D3DXVECTOR3& vPosition, 
		const D3DXVECTOR3& vViewPos, const D3DXVECTOR3& vViewRight, float fRangeSq )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return;

		iter->second->pRenderer->Set3DVolume( vPosition, vViewPos, vViewRight, fRangeSq, NSDSHOW_SOUND::g_nCONTROL_DECIBEL );
	}

	LPDIRECT3DTEXTURE9* DShowTextureMan::GetTexture( const char* szFile )
	{
		MAP_VIDEO_TEXTURE_ITER iter = m_mapVideoTexture.find( szFile );
		if( iter == m_mapVideoTexture.end() )
			return NULL;

		return &(iter->second->pTexture);
	}
};