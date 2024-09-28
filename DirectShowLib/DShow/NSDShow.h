#pragma once

class DShowTexture;

namespace NSDSHOW
{
	extern std::string g_strVideoPath;
};

namespace NSDSHOW_SOUND
{
	extern int g_nCONTROL_DECIBEL;
};

// --------------------------------------------------------------
// Player - 화면 전체에 동영상 재생
namespace NSDSHOW
{
	// Player 초기화 ( long lMsg - 재생 완료 메시지 )
	void InitPlayer( HWND hWnd, long lMsg, LPDIRECT3DDEVICE9 pDevice );

	// 로드 & 재생
	HRESULT LoadAndPlay( const char* szFile );

	// 재생 완료 되었는지 확인
	bool IsPlayerPlayEnd();

	void ReleasePlayer();

	void OnResetDevicePlayer();
	void OnLostDevicePlayer();

	// 볼륨 설정
	void SetPlayerVolume( WORD wVolume );
};

// --------------------------------------------------------------
// Texture - 텍스처에 동영상 렌더링
namespace NSDSHOW
{
	struct SDshowTexture
	{
		LPDIRECT3DTEXTURE9	pTexture;
		LPDIRECT3DSURFACE9	pSurface;
		DShowTexture*		pRenderer;

		SDshowTexture();
		~SDshowTexture();
	};
	// -----------------------------------------

	class DShowTextureMan
	{
	private :
		DShowTextureMan(void);
		~DShowTextureMan(void);
		DShowTextureMan( const DShowTextureMan& value );

	public :
		static DShowTextureMan& GetInstance();

	private :
		typedef std::map< std::string, SDshowTexture* >	MAP_VIDEO_TEXTURE;
		typedef MAP_VIDEO_TEXTURE::iterator				MAP_VIDEO_TEXTURE_ITER;

	private :
		LPDIRECT3DDEVICE9 m_pDevice;

		MAP_VIDEO_TEXTURE m_mapVideoTexture;

	public :
		void OnCreateDevice( LPDIRECT3DDEVICE9 pDevice );
		void OnResetDevice();
		void OnLosetDevice();
		void OnDestroyDevice();

	public :
		// 영상 텍스처 생성 
		// E_POINTER : 디바이스 포인터 없음.
		// S_FALSE : 이미 생성되어 있음.
		// E_FAIL : 생성 실패
		HRESULT CreateVideoTexture( const char* szFile, LPDIRECT3DTEXTURE9* ppTexture );
		HRESULT CreateVideoTexture( const char* szFile, LPBYTE pBuffer, LONGLONG llLength, 
			LPDIRECT3DTEXTURE9* ppTexture );

		// 재생이 끝났는지 확인하고 끝났다면 처음부터 다시 재생
		void ReplayVideo( const char* szFile );

	public :
		void PlayVideo( const char* szFile );
		void PauseVideo( const char* szFile );
		void StopVideo( const char* szFile );
		void StopAllVideo();

		void SetVolume( const char* szFile, WORD wVolume );
		void Set3DVolume( const char* szFile, const D3DXVECTOR3& vPosition, 
			const D3DXVECTOR3& vViewPos, const D3DXVECTOR3& vViewRight, float fRangeSq );

	public :
		LPDIRECT3DTEXTURE9* GetTexture( const char* szFile );
	};
};