#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/DxMethods.h"
#include "./NSBillboardLua.h"

#include "DxBillboard.h"


BILLBOARD_CHF_DATA::BILLBOARD_CHF_DATA()
	: m_fWidth(8.f)
	, m_fHeight(19.f)
	, m_fRightOffset(0.f)
	, m_fUpOffset(0.f)
	, m_nTexSizeIndexWidth(7)
	, m_nTexSizeIndexHeight(8)
{
}

void BILLBOARD_CHF_DATA::Save( const char* szFile )
{
	// 폴더가 없을 수도 있으므로 만들도록 한다.
	CreateDirectory ( NSBillboardLua::GetPath(), NULL );

	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, NSBillboardLua::GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

	sc::SerialFile SFile;

	if ( !SFile.OpenFile ( FOT_WRITE, szPathName ) )		
		return;

	SFile << static_cast<DWORD>( VERSION );
	SFile.BeginBlock();
	{
		SFile << m_fWidth;
		SFile << m_fHeight;
		SFile << m_fRightOffset;
		SFile << m_fUpOffset;
		SFile << m_nTexSizeIndexWidth;
		SFile << m_nTexSizeIndexHeight;
	}
	SFile.EndBlock();

	SFile.CloseFile();
}

BOOL BILLBOARD_CHF_DATA::Load( const char* szFile )
{
	char szPathName[MAX_PATH] = "";
	StringCchCopy( szPathName, MAX_PATH, NSBillboardLua::GetPath() );
	StringCchCat( szPathName, MAX_PATH, szFile );

	sc::SerialFile SFile;

	if ( !SFile.OpenFile ( FOT_READ, szPathName ) )	
		return FALSE;

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_fWidth;
	SFile >> m_fHeight;
	SFile >> m_fRightOffset;
	SFile >> m_fUpOffset;
	SFile >> m_nTexSizeIndexWidth;
	SFile >> m_nTexSizeIndexHeight;

	SFile.CloseFile();

	return TRUE;
}




//////////////////////////////////////////////////////////////////////////
//							BILLBOARD_CHF
BILLBOARD_CHF::BILLBOARD_CHF( const BILLBOARD_CHF_DATA& sSrc, const TSTRING &strTexName )
	: m_fWidth(sSrc.m_fWidth)
	, m_fHeight(sSrc.m_fHeight)
	, m_fRightOffset(sSrc.m_fRightOffset)
	, m_fUpOffset(sSrc.m_fUpOffset)
{
	m_textureRes = TextureManager::GetInstance().LoadTexture
	(
		strTexName,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_EFFECT,
		true,
		FALSE
	);

	m_dwTexSizeX = static_cast<DWORD>( powl( 2, sSrc.m_nTexSizeIndexWidth ) );
	m_dwTexSizeY = static_cast<DWORD>( powl( 2, sSrc.m_nTexSizeIndexHeight ) );
}

void BILLBOARD_CHF::Render( const D3DXVECTOR3 &vPos, float fBillboardAlpha )
{
	if ( !m_textureRes.GetTexture() )
		return;

	const float fXSize = m_fWidth, fYSize = m_fHeight;

	fBillboardAlpha *= 255.f;
	DWORD dwBillboardAlpha = static_cast<DWORD>( fBillboardAlpha );
	if ( dwBillboardAlpha > 255 )
		dwBillboardAlpha = 255;

	if ( dwBillboardAlpha == 0 )
		return;

	VERTEXCOLORTEX vertices[4];
	vertices[0].dwColor = vertices[1].dwColor = vertices[2].dwColor = vertices[3].dwColor = D3DCOLOR_ARGB( dwBillboardAlpha, 255, 255, 255 );
	vertices[0].vTex = D3DXVECTOR2( 0.f, 0.f );
	vertices[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	vertices[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	vertices[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	//for ( DWORD i=0; i<m_vecPosition.size(); ++i )
	//{
	//	const D3DXVECTOR3& vPos = m_vecPosition[i];

		D3DXMATRIX	matLocal;
		DxBillboardLookAt( matLocal, &vPos, DxViewPort::GetInstance().GetMatView() );
		D3DXVECTOR3 vLocal = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		// Note : 점들의 적용
		vLocal.x = -fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[0].vPos, &vLocal, &matLocal );
		vertices[0].vPos.y = fYSize + vPos.y;
		vertices[0].vPos.y += m_fUpOffset;

		vLocal.x = fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[1].vPos, &vLocal, &matLocal );
		vertices[1].vPos.y = fYSize + vPos.y;
		vertices[1].vPos.y += m_fUpOffset;

		vLocal.x = -fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[2].vPos, &vLocal, &matLocal );
		vertices[2].vPos.y = vPos.y;
		vertices[2].vPos.y += m_fUpOffset;

		vLocal.x = fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[3].vPos, &vLocal, &matLocal );
		vertices[3].vPos.y = vPos.y;
		vertices[3].vPos.y += m_fUpOffset;

		if ( dwBillboardAlpha == 255 )
		{
			DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_PDT_HARD_ALPHA.GetFVFSize();
			if( !DxDynamicVB::g_sVB_PDT_HARD_ALPHA.IsValidSize( dwVertexSizeFULL ) )
				return;

			if( !DxDynamicVB::g_sVB_PDT_HARD_ALPHA.IsValidCopy( dwVertexSizeFULL ) )
				OPTMManager::GetInstance().Render();

			UINT nVertexCount = DxDynamicVB::g_sVB_PDT_HARD_ALPHA.CopyToVB
			(
				vertices,
				dwVertexSizeFULL,
				4 
			);

			// Note : 데이터를 넣는다.
			OPTMSingleBillboard::DATA* pData = OPTMManager::GetInstance().m_sSingleBillboard.GetData();
			pData->pTex = m_textureRes.GetTexture();
			pData->nVertexCount = nVertexCount;
			pData->nFaceCount = 2;

			// Note : 나중에 뿌리도록 셋팅 해 놓는다.
			OPTMManager::GetInstance().m_sSingleBillboard.InsertData( pData );
		}
		else
		{
			DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_PDT.GetFVFSize();
			if( !DxDynamicVB::g_sVB_PDT.IsValidSize( dwVertexSizeFULL ) )
				return;

			if( !DxDynamicVB::g_sVB_PDT.IsValidCopy( dwVertexSizeFULL ) )
				OPTMManager::GetInstance().Render();

			UINT nVertexCount = DxDynamicVB::g_sVB_PDT.CopyToVB
			(
				vertices,
				dwVertexSizeFULL,
				4 
			);

			// Note : 데이터를 넣는다.
			OPTMSingleSequence::DATA* pData = OPTMManager::GetInstance().m_sSingleSequence.GetData();
			pData->pTex = m_textureRes.GetTexture();
			pData->nVertexCount = nVertexCount;
			pData->nFaceCount = 2;

			// Note : 나중에 뿌리도록 셋팅 해 놓는다.
			OPTMManager::GetInstance().m_sSingleSequence.InsertData( pData );
		}
//	}
}
