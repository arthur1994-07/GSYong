#include "pch.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../G-Logic/GLOGIC.h"
#include "../../G-Logic/GLDefine.h"
#include "../../Common/StlFunctions.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../Char/DxEffcharData.h"
#include "./DxEffSinglePropGMan.h"
#include "./DxEffectParticleSys.h"
#include "./DxEffGroupPlayer.h"

#include ".\dxeffbillboardchar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxEffBillboardChar& DxEffBillboardChar::GetInstance()
{
	static DxEffBillboardChar Instance;
	return Instance;
}

DxEffBillboardChar::DxEffBillboardChar(void)
{
}

DxEffBillboardChar::~DxEffBillboardChar(void)
{
}

HRESULT DxEffBillboardChar::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

HRESULT DxEffBillboardChar::LoadBillboardTex ( const std::string *pstrCharBoard )
{
	for( int i = 0; i < EMBOARD_SIZE; i++ )
	{
		m_strTexture[i] = pstrCharBoard[i];
	}

	return S_OK;
}

HRESULT DxEffBillboardChar::RestoreDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffBillboardChar::InvalidateDeviceObjects ()
{
	return S_OK;
}

bool SBillboardSort::operator() ( const SBillboadCharInfo &a, const SBillboadCharInfo &b )
{
	D3DXVECTOR3 vLength1 = DxViewPort::GetInstance().GetFromPt() - a.vPos;
	D3DXVECTOR3 vLength2 = DxViewPort::GetInstance().GetFromPt() - b.vPos;
	float fLength1 = D3DXVec3Length( &vLength1 );
	float fLength2 = D3DXVec3Length( &vLength2 );

	return fLength1 < fLength2 ? TRUE : FALSE;
}

HRESULT DxEffBillboardChar::DeleteDeviceObjects ()
{
	for( int i = 0; i < EMBOARD_SIZE; i++ )
		m_textureRes[i].Clear();

	return S_OK;
}

void DxEffBillboardChar::AddBillboardChar( EMBOARD_TYPE type, const D3DXVECTOR3 &vPos )
{
	SBillboadCharInfo boardCharInfo;
	boardCharInfo.emType = type;
	boardCharInfo.vPos   = vPos;
	m_vecBoardCharinfo.push_back( boardCharInfo );
}

void DxEffBillboardChar::FrameMove( float fTime, float fElapsedTime )
{
	m_vecBoardCharinfo.clear();
}

void DxEffBillboardChar::Render()
{
	const float fXSize = 9.0f, fYSize = 18.0f;
	D3DXVECTOR3 vPos;

	std::sort( m_vecBoardCharinfo.begin(), m_vecBoardCharinfo.end(), SBillboardSort() );
	
	VERTEXCOLORTEX vertices[4];
	vertices[0].dwColor = vertices[1].dwColor = vertices[2].dwColor = vertices[3].dwColor = 0xffffffff;
	vertices[0].vTex = D3DXVECTOR2( 0.f, 0.f );
	vertices[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	vertices[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	vertices[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	for( DWORD i = 0; i < m_vecBoardCharinfo.size(); i++ )
	{
		const SBillboadCharInfo& boardCharInfo = m_vecBoardCharinfo[i];

		// �ؽ��� �迭�� ���� ���� �ʴµ� �ǵ���� ����������.

		if( EMBOARD_SIZE < boardCharInfo.emType )	// �迭����Ʈ���� �� ũ�ٸ� ������ ������ �ϴ� Continue�� ���� �޸� ������ �ȳ������� 
			continue ;

		if( !m_textureRes[boardCharInfo.emType].GetTexture() )
		{
			// [shhan][2014.12.10] �������� ������ �ε� �س�����. Memory �� ��Ƹ԰� �Ǿ �����Ҵ�.
			//						Texture �� ������ �ȵǴ� �Ŷ� Render �ȿ� Texture ���� ��� �ε��ϵ��� �ڵ��Ͽ���.
			m_textureRes[boardCharInfo.emType] = TextureManager::GetInstance().LoadTexture( m_strTexture[boardCharInfo.emType],
																							false,
																							TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
																							EMMM_CHAR_MIPMAP,
																							D3DFMT_UNKNOWN,
																							TEXTURE_RESIZE_NONE,
																							TEXTURE_CHAR,
																							false,
																							FALSE );
            continue;
		}

		vPos = boardCharInfo.vPos;

		D3DXMATRIX	matLocal;
		DxBillboardLookAt( matLocal, &vPos, DxViewPort::GetInstance().GetMatView() );
		D3DXVECTOR3 vLocal = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

		// Note : ������ ����			
		vLocal.x = -fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[0].vPos, &vLocal, &matLocal );
		vertices[0].vPos.y = fYSize + vPos.y;

		vLocal.x = fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[1].vPos, &vLocal, &matLocal );
		vertices[1].vPos.y = fYSize + vPos.y;

		vLocal.x = -fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[2].vPos, &vLocal, &matLocal );
		vertices[2].vPos.y = vPos.y;

		vLocal.x = fXSize/2.0f;
		D3DXVec3TransformCoord( &vertices[3].vPos, &vLocal, &matLocal );
		vertices[3].vPos.y = vPos.y;

		DWORD dwVertexSizeFULL = 4*DxDynamicVB::g_sVB_PDT.GetFVFSize();
        if( !DxDynamicVB::g_sVB_PDT.IsValidSize( dwVertexSizeFULL ) )
            continue;

        if( !DxDynamicVB::g_sVB_PDT.IsValidCopy( dwVertexSizeFULL ) )
			OPTMManager::GetInstance().Render();

        UINT nVertexCount = DxDynamicVB::g_sVB_PDT.CopyToVB(
		    vertices,
		    dwVertexSizeFULL,
		    4 );

		// Note : �����͸� �ִ´�.
        OPTMSingleSequence::DATA* pData = OPTMManager::GetInstance().m_sSingleSequence.GetData();
		pData->pTex = m_textureRes[boardCharInfo.emType].GetTexture();
        pData->nVertexCount = nVertexCount;
		pData->nFaceCount = 2;

		// Note : ���߿� �Ѹ����� ���� �� ���´�.
		OPTMManager::GetInstance().m_sSingleSequence.InsertData( pData );
	}
}
