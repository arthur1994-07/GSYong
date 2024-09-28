#pragma once

#include <map>
#include <string>
#include <boost/tr1/unordered_map.hpp>

#include "../../SigmaCore/File/FileFind.h"
#include "../../SigmaCore/gassert.h"
#include "../../InternalCommonLib/dxstdafx.h"

#include "../Common/StlFunctions.h"
#include "../DxCommon9/dxstdafx.h"
#include "../ResourceManagerCacheAlgorithm.h"
#include "../TextureMemory.h"

struct BoxSize
{
	BoxSize() {width = 0; height = 0;}
	BoxSize(UINT width,UINT height) { this->width = width; this->height = height; }
	UINT width,height;
};

class CompletedTextureData
{
public:
	
	~CompletedTextureData()
	{
		if( m_pD3DBaseTexture != NULL )
        {
            TextureMemory::OnRelease( m_pD3DBaseTexture, m_emCategory );
			m_pD3DBaseTexture->Release(); 
        }
	}

	CompletedTextureData(
        IDirect3DBaseTexture9* pD3DBaseTexture,
		const TSTRING& strName,
        const TEXTURE_ADD_DATA& addData,
		const D3DXIMAGE_INFO& info,
        TEXTURE_ALPHA emTextureAlphaType,
        TEXTURE_RESIZE textureResizeType,
        TEXTURE_CATEGORY emCategory )
	{
		m_pD3DBaseTexture = pD3DBaseTexture;
		m_strName = strName;
		m_addData =	addData;
		m_imageInfo = info;
		m_emTextureAlphaType = emTextureAlphaType;
		m_textureResizeType = textureResizeType;
        m_emCategory = emCategory;
		m_bBackUpCache = addData.m_bBackUpCache;
		m_bLogWrite = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	void DataExchange(CompletedTextureData& rhs);

	//////////////////////////////////////////////////////////////////////////
	LPDIRECT3DTEXTUREQ GetTexture();
	LPDIRECT3DCUBETEXTUREQ GetCubeTexture();
	
	//////////////////////////////////////////////////////////////////////////
	const TEXTURE_ADD_DATA& GetAddData() { return m_addData; }
	const D3DXIMAGE_INFO& GetInfo() const { return m_imageInfo; }
	TEXTURE_ALPHA GetEmTextType() const { return m_emTextureAlphaType; }
	TEXTURE_RESIZE   GetTextureResizeType() { return m_textureResizeType; }
    TEXTURE_CATEGORY GetTextureCategory() { return m_emCategory; }
	BOOL IsBackUpCache() { return m_bBackUpCache; }

private:
	//////////////////////////////////////////////////////////////////////////

	IDirect3DBaseTexture9* m_pD3DBaseTexture;
	TSTRING					m_strName;
	TEXTURE_ADD_DATA	   m_addData;
	D3DXIMAGE_INFO         m_imageInfo;
	TEXTURE_ALPHA  m_emTextureAlphaType;
	TEXTURE_RESIZE    m_textureResizeType;
    TEXTURE_CATEGORY m_emCategory;
	BOOL			m_bBackUpCache;	//해진씨가 기능 만들어 줄때까지 임시로 쓰도록 하자.
	BOOL			m_bLogWrite;
};


// TAG_ClassTextureManager_1
// TAG_CreateResourceManager_1
//public DefaultTimeCacheAlogorithm<CompletedTextureData,TEXTURE_ADD_DATA>
/// 현재 게임에서 쓰이는 특성상  캐쉬정책은 OnceRefAndEnableAlgorithm를 쓰는 리소스 메니저를 상속받았다
namespace NSTEXTUREMAN
{
class TextureManager :  public OnceRefAndEnableAlgorithm<CompletedTextureData,TEXTURE_ADD_DATA,OnceRefNTime>
{
private:
	TextureManager();

	// 백그라운드 쓰레드 로우 데이타를 전처리해줄게 있다면 여기서 해주면 된다 단 멀티 쓰레드임을 잊지 말자
	virtual void PreprocessRawDataWidthBackgroundThread(SP_RAW_DATA& spData,UINT& size,const void* pAddData) {}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 로우 데이터에서 완전한 텍스쳐 데이터로 거듭나게 만들어주는곳
	virtual void ProcessCompleteData(
        SP_QUEUE_DATA_VECTOR& dataVec,
        NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );
	//////////////////////////////////////////////////////////////////////////

	virtual void ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec );

public:

	// 이건 좀더 쉽게 사용하기 위한 랩퍼 역활정도
	// 여기 데이터는 자동 관리되기때문에 절대 내부 데이터를 강제로 지우려거나 Release 시키려 하지 말것!
	struct Resource : public ResourceManager::Resource
	{
		TEXTURE_ADD_DATA m_sTextureAddDataTemp;
		D3DXIMAGE_INFO m_sD3DXImageInfoTemp;

		/// Create로 생성된넘을 자동 관리로 넣어주기 위한 생성자. 
		/// Load랑 Create랑 혼용해서 쓰면 안되는데 create로 생성이 필요한 경우  이 생성자로 담아두면
		/// Load로 가져온거랑 똑같이 사용할수 있다
		explicit Resource(
            IDirect3DBaseTexture9* pD3DBaseTexture,
            const TSTRING& fileName,
			TEXTURE_ADD_DATA::TYPE type = TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			const D3DXIMAGE_INFO& info = D3DXIMAGE_INFO(),
            TEXTURE_ALPHA emTextureAlphaType = EMTT_NORMAL,
			TEXTURE_RESIZE textureResizeType = TEXTURE_RESIZE_NONE,
            TEXTURE_CATEGORY emCategory = TEXTURE_ETC )
		{
			m_spCompletedData = SP_COMPLETED_DATA(
				new CompletedTextureData(
                    pD3DBaseTexture,
					fileName,
                    type,
                    info,
                    emTextureAlphaType,
                    textureResizeType,
                    emCategory ) );

			memset( &m_sD3DXImageInfoTemp, 0, sizeof( m_sD3DXImageInfoTemp ) );
		}

		Resource( const ResourceManager::Resource& res )
			: ResourceManager::Resource( res )
		{
			memset( &m_sD3DXImageInfoTemp, 0, sizeof( m_sD3DXImageInfoTemp ) );
		}

		Resource()
		{
			memset( &m_sD3DXImageInfoTemp, 0, sizeof( m_sD3DXImageInfoTemp ) );
		}
	
		LPDIRECT3DTEXTUREQ GetTexture()  { return Update() ? m_spCompletedData->GetTexture() : NULL; }
		LPDIRECT3DCUBETEXTUREQ GetCubeTexture() { return Update() ? m_spCompletedData->GetCubeTexture() : NULL; }
		CompletedTextureData* Get()  { return Update() ? m_spCompletedData.get() : NULL; }
		const TEXTURE_ADD_DATA& GetAddData() { return Update() ? m_spCompletedData->GetAddData() : m_sTextureAddDataTemp; }
		const D3DXIMAGE_INFO& GetInfo() { return Update() ? m_spCompletedData->GetInfo() : m_sD3DXImageInfoTemp; }
		TEXTURE_ALPHA GetAlphaType() { return Update() ? m_spCompletedData->GetEmTextType() : EMTT_NORMAL; }

		LPDIRECT3DTEXTUREQ	GetTexture_const() const	{ return IsCompletedData() ? m_spCompletedData->GetTexture() : NULL; }
		TEXTURE_ALPHA		GetAlphaType_const() const	{ return IsCompletedData() ? m_spCompletedData->GetEmTextType() : EMTT_NORMAL; }
	};

public:
	static TextureManager& GetInstance();

	TSTRING GetTexturePathFileFromFile(const TSTRING& fileName) { return QueryPathFileNameHelper(_T(""),_T(""),fileName); }

	void SetForceLessTexture( bool bForce ) { m_bForcelessThan1024 = bForce; }
    bool GetForceLessTexture() { return m_bForcelessThan1024; }

    void SetPath(const TSTRING& path);
	void RescanPath();
	
	const TSTRING& GetPath() { return m_fileTree.GetPath(); }

	// 들어온 확장자와 mtf 모두 검색하도록 한다.
	bool IsPathFile(const TSTRING& fileName );

    bool FindFilePath( TSTRING& strFilePath );

	void SetDevice(LPDIRECT3DDEVICEQ pDevice) { m_pD3DDevice = pDevice; }
	void EnableTextureAlphaTypeDebug() { m_bTextureAlphaTypeDebug = true; }

	/// 관리되는 리소스 
	/// @remark  백그라운드 로딩일 경우 Resource::Update() 함수가 true가 될때가지 실제로 참조가 이루어지는게
	///			 아님
	
	void EnableLog(bool bEnble) { m_bEnableLog = bEnble; }

	Resource LoadTexture(
        const TSTRING& pathFileName,
		bool bMustDirectory/* = false*/,
		TEXTURE_ADD_DATA::TYPE textureType/* = TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D*/,
		DWORD mipMap/* = 0*/,
		D3DFORMAT d3dFormat/* = D3DFMT_UNKNOWN*/,
		TEXTURE_RESIZE resizeType/* = TEXTURE_RESIZE_NONE*/,
        TEXTURE_CATEGORY emCategory/* = TEXTURE_ETC*/,
		bool bBackgroundLoading/* = false*/,
		BOOL bBackUpCache/* = FALSE*/);

	bool ReLoadTexture(
		const TSTRING& pathFileName,
		Resource& kRes,
		bool bMustDirectory/* = false*/,
		TEXTURE_ADD_DATA::TYPE textureType/* = TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D*/,
		DWORD mipMap/* = 0*/,
		D3DFORMAT d3dFormat/* = D3DFMT_UNKNOWN*/,
		TEXTURE_RESIZE resizeType/* = TEXTURE_RESIZE_NONE*/,
		TEXTURE_CATEGORY emCategory/* = TEXTURE_ETC*/,
		bool bBackgroundLoading/* = false*/,
		BOOL bBackUpCache/* = FALSE*/);

	bool ReLoadData(const TSTRING& pathFileName,Resource& kRes,bool bMustDirectory,const TEXTURE_ADD_DATA& addData,bool bBackgroundLoading = false);

	// Clear 함수는 리소스 메니저에서 상속받음


	//////////////////////////////////////////////////////////////////////////
	// 관리 되지않는 로우레벨 다이렉트 호출

    void ReleaseTexture( LPDIRECT3DTEXTURE9& pTexture, TEXTURE_CATEGORY emCategory );
    void ReleaseTexture( LPDIRECT3DCUBETEXTURE9& pTexture, TEXTURE_CATEGORY emCategory );

	/// @remark  이건 ReleaseTexture로 자원을 해제해야함
	HRESULT CreateTexture(
		LPDIRECT3DDEVICEQ pDevice,
		UINT& uiWidth,
		UINT& uiHeight,
		UINT uiMipLevels,
		DWORD dwUsage,
		D3DFORMAT& emFormat,
		D3DPOOL emPool,
		LPDIRECT3DTEXTUREQ* ppTexture,
		TEXTURE_RESIZE resizeType,
        TEXTURE_CATEGORY emCategory );

	/// @remark  이건 ReleaseTexture로 자원을 해제해야함
	HRESULT CreateTextureFromFileEx(
		LPDIRECT3DDEVICE9 pDevice,
		LPCSTR pSrcFile,
		UINT& uiWidth,
		UINT& uiHeight,
		UINT uiMipLevels,
		DWORD dwUsage,
		D3DFORMAT& emFormat,
		D3DPOOL emPool,
		DWORD dwFilter,
		DWORD dwMipFilter,
		D3DCOLOR dwColorKey,
		D3DXIMAGE_INFO* pSrcInfo,
		PALETTEENTRY* pPalette,
		LPDIRECT3DTEXTUREQ* ppTexture,
		TEXTURE_RESIZE resizeType,
        TEXTURE_CATEGORY emCategory );

	/// @remark  이건 ReleaseTexture로 자원을 해제해야함
	HRESULT CreateTextureFromFileInMemoryEx(
		LPDIRECT3DDEVICE9 pDevice,
		LPCVOID pSrcData,
		UINT uiSrcDataSize,
		UINT& uiWidth,
		UINT& uiHeight,
		UINT uiMipLevels,
		DWORD dwUsage,
		D3DFORMAT& emFormat,
		D3DPOOL emPool,
		DWORD dwFilter,
		DWORD dwMipFilter,
		D3DCOLOR dwColorKey,
		D3DXIMAGE_INFO* pSrcInfo,
		PALETTEENTRY* pPalette,
		LPDIRECT3DTEXTUREQ* ppTexture,
		TEXTURE_RESIZE resizeType,
        TEXTURE_CATEGORY emCategory );

	TEXTURE_ALPHA GetTexType(const TSTRING& strName );

	BoxSize GetForceLessSize(
        UINT width,
        UINT height,
        TEXTURE_RESIZE resizeType,
        const TSTRING& fileName );

private:
	TEXTURE_ALPHA GetCompleteTex_TexType( const TSTRING& strName );
	//	bool GetCompleteTex_ImageInfo(const TSTRING& strName, D3DXIMAGE_INFO& sInfo );

private:
	BoxSize GetForceLessSize(
        const TSTRING& fileName,
        D3DXIMAGE_INFO* pImageInfo,
		TEXTURE_RESIZE resizeType,
        const void* pRawData = NULL,
        UINT rawDataSize = 0);

	virtual TSTRING QueryPathFileNameHelper(
        const TSTRING& pathFileName,
		const TSTRING& path,
        const TSTRING& fileName );

    void FindAndChangeExtMTF( TSTRING& strFilePath );

private:
    sc::CFileFindTree m_fileTree;
	LPDIRECT3DDEVICEQ m_pD3DDevice;
	bool m_bEnableLog;
	bool m_bForcelessThan1024;      // 강제로 크기를 줄인다
	bool m_bTextureAlphaTypeDebug;
    DWORD m_nMemoryBytes[ TEXTURE_CATEGORY_SIZE ];

public:
    void Archive(boost::function<void (const TCHAR*)> fun);
};
}

using namespace NSTEXTUREMAN;
typedef NSTEXTUREMAN::TextureManager::Resource TextureResource;

namespace TextureManagerUtil
{
    // 모든 텍스쳐의 Mipmap 을 검사한다.
    void AllTextureCheckMipMap( LPDIRECT3DDEVICEQ pd3dDevice );
}