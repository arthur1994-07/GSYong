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
// Player - ȭ�� ��ü�� ������ ���
namespace NSDSHOW
{
	// Player �ʱ�ȭ ( long lMsg - ��� �Ϸ� �޽��� )
	void InitPlayer( HWND hWnd, long lMsg, LPDIRECT3DDEVICE9 pDevice );

	// �ε� & ���
	HRESULT LoadAndPlay( const char* szFile );

	// ��� �Ϸ� �Ǿ����� Ȯ��
	bool IsPlayerPlayEnd();

	void ReleasePlayer();

	void OnResetDevicePlayer();
	void OnLostDevicePlayer();

	// ���� ����
	void SetPlayerVolume( WORD wVolume );
};

// --------------------------------------------------------------
// Texture - �ؽ�ó�� ������ ������
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
		// ���� �ؽ�ó ���� 
		// E_POINTER : ����̽� ������ ����.
		// S_FALSE : �̹� �����Ǿ� ����.
		// E_FAIL : ���� ����
		HRESULT CreateVideoTexture( const char* szFile, LPDIRECT3DTEXTURE9* ppTexture );
		HRESULT CreateVideoTexture( const char* szFile, LPBYTE pBuffer, LONGLONG llLength, 
			LPDIRECT3DTEXTURE9* ppTexture );

		// ����� �������� Ȯ���ϰ� �����ٸ� ó������ �ٽ� ���
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