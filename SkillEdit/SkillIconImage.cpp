#include "stdafx.h"

#include <shlwapi.h>

#include "SkillIconImage.h"

#include "../EngineLib/DxBase/RenderDevice.h"

#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "../RanLogic/Skill/GLSkill.h"

#include "../SigmaCore/DebugInclude.h"

SkillIconImage::SkillIconImage()
: m_hWndTarget( NULL )
, m_sIconID( false ) 
{
	m_vecError.clear();
}

SkillIconImage::~SkillIconImage()
{
	m_vecError.clear();
}

void SkillIconImage::AttachWnd( HWND hWndTarget )
{
	m_hWndTarget = hWndTarget;

	CD3DViewport::WindowProc( WM_CREATE, 0, 0 );
}

void SkillIconImage::DetachWnd()
{
	CD3DViewport::WindowProc( WM_DESTROY, 0, 0 );
}

VOID SkillIconImage::OnFrameRender( IDirect3DDevice9* pd3dDevice
								   , DOUBLE fTime
								   , FLOAT fElapsedTime )
{
	const CRect& cSourceRect = GetSourceRect();

	BeginScene( pd3dDevice, D3DCOLOR_XRGB( 0, 0, 0 ) );

	CTextUtil::Get()->Render( TRUE );

	if( m_strFileName.empty() /*|| ( m_sIconID == NATIVEID_NULL() )*/ )
	{
		CTextUtil::Get()->Render( FALSE );
		EndScene( pd3dDevice );
		return;
	}

	g_pRI->SetOrigin( m_sOrigin );
	g_pRI->SetZoom( m_fZoom );

	g_pRI->SetViewport(
		pd3dDevice,
		cSourceRect.left,
		cSourceRect.top,
		cSourceRect.Width(),
		cSourceRect.Height() );

	TextureResource textureRes = 
		TextureManager::GetInstance().LoadTexture(
		m_strFileName.c_str(),
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_UI,
		false,
		FALSE );

	if( textureRes.GetTexture() )
	{
		const D3DXIMAGE_INFO& sImageInfo = textureRes.GetInfo();

		D3DXVECTOR2 vTexPos( 0.0f, 0.0f );
		//vTexPos.x = FLOAT( m_sIconID.wMainID * EIconSize ) / FLOAT( sImageInfo.Width );
		//vTexPos.y = FLOAT( m_sIconID.wSubID * EIconSize ) / FLOAT( sImageInfo.Height );

		D3DXVECTOR2 vTexSize( 1.0f, 1.0f );
		//vTexSize.x = FLOAT( EIconSize ) / FLOAT( sImageInfo.Width );
		//vTexSize.y = FLOAT( EIconSize ) / FLOAT( sImageInfo.Height );

		g_pRI->DrawTile(
			ic::Int2( 0, 0 ),
			ic::Int2( EIconSize, EIconSize ),
			vTexPos,
			vTexSize,
			D3DCOLOR_XRGB( 255, 255, 255 ),
			textureRes.GetTexture(),
			FALSE );

		g_pRI->FlushBatchedPrimitives();
	}

	CTextUtil::Get()->Render( FALSE );

	EndScene( pd3dDevice );
}

void SkillIconImage::SaveIconToFile( const CString& strSavePath, const D3DXIMAGE_FILEFORMAT SaveType )
{
	m_vecError.clear();

	WORD wMaxMID, wMaxSID;
	GLSkillMan::GetInstance().GetMaxSkill ( wMaxMID, wMaxSID );

	for( WORD wMID = 0; wMID < wMaxMID; ++wMID)
	{
		for( WORD wSID = 0; wSID < wMaxSID; ++wSID )
		{
			SaveIconToFile(strSavePath, wMID, wSID, SaveType);
		}
	}
}

void SkillIconImage::SaveIconToFile( const CString& strSavePath, const WORD wMID, const WORD wSID, const D3DXIMAGE_FILEFORMAT SaveType)
{
	GLSKILL *pSkill = GLSkillMan::GetInstance().GetData (wMID, wSID);
	
	if(pSkill)
	{
		SNATIVEID sICONINDEX = pSkill->m_sEXT_DATA.sICONINDEX;
		std::string strTextureName( pSkill->m_sEXT_DATA.strICONFILE );

		TextureResource textureRes = 
			TextureManager::GetInstance().LoadTexture(
			strTextureName.c_str(),
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_UI,
			false,
			FALSE );
		
		if( textureRes.GetTexture() )
		{
			//아이콘 추출
			LPDIRECT3DTEXTURE9	SaveTexture = NULL;
			D3DXVECTOR2 vTexPos, vIconSize( SkillIconImage::EIconSize, SkillIconImage::EIconSize );
			vTexPos.x = static_cast<float>( sICONINDEX.wMainID * SkillIconImage::EIconSize );
			vTexPos.y = static_cast<float>( sICONINDEX.wSubID * SkillIconImage::EIconSize );

			if( !ExtractIcon( SaveTexture, textureRes.GetTexture(), vTexPos, vIconSize ) )
				return;
			
			// 사용자 정의 파일명 생성
			CString strResultPath, strFileName;
			CreateCustomFileName(strFileName, pSkill->m_sBASIC.szNAME, strTextureName.c_str(), sICONINDEX, SaveType);
			
			//최종 저장 경로 및 파일명
			strResultPath = strSavePath + strFileName;

			//정상적인 경로가 들어와야 하지만 생성되지 않은 폴더일 때는 생성을 시도한다. 
			//if( CreateDirectoryFunc(strSavePath) )
				D3DXSaveTextureToFile(strResultPath, SaveType, SaveTexture, NULL);
				
			if( SaveTexture ) SaveTexture->Release();
		}
		else//아이콘이 없는 스킬들 아이디저장 에러 출력용
		{
			m_vecError.push_back(pSkill->m_sBASIC.szNAME);
		}
	}
	
}

bool SkillIconImage::ExtractIcon( LPDIRECT3DTEXTURE9 &ResultTexture, const LPDIRECT3DTEXTURE9 &TargetTexture, const D3DXVECTOR2 &vIconPos, const D3DXVECTOR2 &vIconSize )
{
	D3DXCreateTexture( CRenderDevice::s_pd3dDevice, vIconSize.x, vIconSize.y, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &ResultTexture);

	if(ResultTexture)
	{
		LPDIRECT3DSURFACE9  pTargetSurf = NULL;	
		ResultTexture->GetSurfaceLevel(0, &pTargetSurf);

		LPDIRECT3DSURFACE9  pSrcSurf = NULL ;
		TargetTexture->GetSurfaceLevel(0, &pSrcSurf);        

		RECT SrcRect;
		SrcRect.left	= static_cast<LONG>( vIconPos.x ); 
		SrcRect.right	= static_cast<LONG>( vIconPos.x +  vIconSize.x ); 
		SrcRect.top		= static_cast<LONG>( vIconPos.y); 
		SrcRect.bottom	= static_cast<LONG>( vIconPos.y +  vIconSize.y );

		if(S_OK != D3DXLoadSurfaceFromSurface(pTargetSurf, NULL, NULL, pSrcSurf, NULL, &SrcRect, D3DX_FILTER_LINEAR , 0) )
		{
			pTargetSurf->Release();
			pSrcSurf->Release();
			return false;
		}

		pTargetSurf->Release();
		pSrcSurf->Release();
	}
	else
		return false;

	return true;
}

void SkillIconImage::SaveIconErrorListToFile( const CString& strSavePath, const CString& strFileName, const CString& strComment, bool bSaveCSV )
{
	if( bSaveCSV )
		SaveIconErrorListToCSV( strSavePath, strFileName, strComment );
	else
		SaveIconErrorListToTXT( strSavePath, strFileName, strComment );
	
}

void SkillIconImage::SaveIconErrorListToCSV( const CString& strSavePath, const CString& strFileName, const CString& strComment )
{
	CString strResult = NULL;
	
	strResult = strSavePath + strFileName;

	std::fstream streamFILE;
	streamFILE.open ( strResult, std::ios_base::out );
	
	streamFILE << strComment <<std::endl;

	for( unsigned int i = 0; i < m_vecError.size(); ++i )
		streamFILE << m_vecError[i] << std::endl;

	streamFILE.close();
}

void SkillIconImage::SaveIconErrorListToTXT( const CString& strSavePath, const CString& strFileName, const CString& strComment )
{
	CString strResult = NULL;
	
	strResult = strSavePath + strFileName;

	FILE* fp = _fsopen ( strResult, "wt", SH_DENYWR );
	if ( !fp ) return ;
	
	fprintf ( fp, "%s\n", strComment);

	for( unsigned int i = 0; i < m_vecError.size(); ++i )
		fprintf ( fp, "%s\n", m_vecError[i]);
	
	fclose ( fp );
}

bool SkillIconImage::CreateDirectoryFunc( LPCTSTR path )
{
	TCHAR szPathBuffer[MAX_PATH];

	UINT len = lstrlen(path);
	for(UINT i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(path + i);
		if(szPathBuffer[i] == TEXT('\\') || szPathBuffer[i] == TEXT('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if(!PathFileExists(szPathBuffer))
			{
				if(!CreateDirectory(szPathBuffer, NULL))
				{
					if(GetLastError() != ERROR_ALREADY_EXISTS)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}

bool SkillIconImage::CreateCustomFileName(CString OUT &strResult, const char *szSkillID, const char *szTextureName, const SNATIVEID& sIconIndex, D3DXIMAGE_FILEFORMAT SaveType)
{
	
	//텍스쳐 파일명 확장자 제거
	char    tmp_str[MAX_PATH];
	strcpy(tmp_str, szTextureName);
	char*    pExt = strrchr(tmp_str, '.');
	if (pExt) *pExt = '\0';
	
	//최종 출력 파일명 생성
	strResult.Format(
		"%s(%s,%d,%d)", 
		szSkillID, 
		tmp_str,
		sIconIndex.wMainID, 
		sIconIndex.wSubID
		);

	//확장자 타입 추가
	switch(SaveType)
	{
	case D3DXIFF_BMP:
		strResult += ".bmp";
		break;
	case D3DXIFF_JPG:
		strResult += ".jpg";
		break;
	case D3DXIFF_TGA:
		strResult += ".tga";
		break;
	case D3DXIFF_PNG:
		strResult += ".png";
		break;
	case D3DXIFF_DDS:
		strResult += ".dds";
		break;
	case D3DXIFF_PPM:
		strResult += ".ppm";
		break;
	case D3DXIFF_DIB:
		strResult += ".dib";
		break;
	case D3DXIFF_HDR:
		strResult += ".hdr";
		break;
	case D3DXIFF_PFM:
		strResult += ".pfm";
		break;
	}

	return true;
}

