#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"

#include "../../DxTools/DxGlowMan.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxCubeMap.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/DxEnvironment.h"
#include "../../DxTools/RENDERPARAM.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxMeshs/SkinMesh/DxSkinCharData.h"


#include "./DxEffectSkinMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD SKINMESH_PROPERTY::VERSION = 0x0101;
const char	SKINMESH_PROPERTY::NAME[]   = "��Ų�޽� PLAY";

//	Note: �⺻���� �ʱ�ȭ�� �� ����Ÿ�� ���� ���� Ŭ������ �Ѱ��ش�.
//		  ���� ���õ� ��� ����Ÿ�� �ѱ�� �κ��̴�.
//        �� �κ��� ȣ��Ǳ� �������� ����Ŭ������ �������� �ʴ´�.
DxEffSingle* SKINMESH_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffectSkinMesh *pEffect = new DxEffectSkinMesh;

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;
	
	//	Note : �ð� ����.			[���뼳��]
	//
	pEffect->m_fGBeginTime		= m_fGBeginTime;
	pEffect->m_fGLifeTime		= m_fGLifeTime;		

	//	Note : ���ư��� ������Ʈ	[���뼳��]
	//
	pEffect->m_bMoveObj = m_bMoveObj;
	
	// Note: �ʱ� ���� �κ� 
	pEffect->m_dwFlag = m_dwFlag;

//	pEffect->m_pSkinChar = &m_sSkinChar;
	
	if ( m_pSkinCharData )
	{
		pEffect->m_pSkinChar = new DxSkinChar;
		pEffect->m_pSkinChar->SetCharData ( m_pSkinCharData, m_pd3dDevice, FALSE, EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
		pEffect->m_pSkinChar->DEFAULTANI();
	}
	
	//	Note : ���� ����
	//
	pEffect->m_MovSound = m_MovSound;

	return pEffect;
}

HRESULT SKINMESH_PROPERTY::SaveFile ( sc::SerialFile &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	//	Note : DATA�� ����� ����Ѵ�. Load �ÿ� ������ Ʋ�� ��� ����.
	//
	SFile << (DWORD) ( GetSizeBase() + sizeof(m_Property) );

	//	Note : �θ� Ŭ������ ����.
	SFile.WriteBuffer( m_matLocal, sizeof(D3DXMATRIX) );
	
	SFile << m_bMoveObj;
	SFile << m_fGBeginTime;
	SFile << m_fGLifeTime;

	//	Note : ����Ʈ�� Property �� ����.
	//
	SFile.WriteBuffer ( &m_Property, sizeof(PROPERTY) );

	//	Note : ����, �ڽ� ����.
	//
	EFF_PROPERTY::SaveFile ( SFile );

	return S_OK;
}

HRESULT SKINMESH_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	if ( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else if ( dwVer == 0x0100 )
	{
		//	Note : �θ� Ŭ������ ����.
		DXAFFINEPARTS sAffineParts;
		SFile.ReadBuffer( m_matLocal, sizeof(D3DXMATRIX) );
		SFile.ReadBuffer( &sAffineParts, sizeof(DXAFFINEPARTS) );
		D3DXMatrixCompX( m_matLocal, sAffineParts );
		
		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );
	}
	else
	{
		//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
		//
		DWORD dwCur = SFile.GetfTell ();
		SFile.SetOffSet ( dwCur+dwSize );
	}

	//	Note : Device �ڿ��� �����Ѵ�.
	//
	Create ( pd3dDevice, bBackUpCache );

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT SKINMESH_PROPERTY::InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	m_pd3dDevice = pd3dDevice;

	DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_szMeshFile, pd3dDevice );
	if( pSkinCharData == NULL )
        return E_FAIL;

	m_sSkinChar.SetCharData( pSkinCharData, pd3dDevice, FALSE, EMSCD_ALL_THREAD | EMSCD_SET_BASE_CPS | EMSCD_JIGGLE_BONE );
	m_pSkinCharData = pSkinCharData;

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );	

	return S_OK;
}


//HRESULT SKINMESH_PROPERTY::DeleteDeviceObjects ()
//{	
//	//	Note : ����, �ڽ�.
//	//
//	EFF_PROPERTY::DeleteDeviceObjects ();	
//
//	return S_OK;
//}


//------------------------------- ���� Ŭ����(CMeshEffect)���� ----------------------------------//
const DWORD DxEffectSkinMesh::TYPEID = EFFSINGLE_SKINMESH;
const DWORD DxEffectSkinMesh::FLAG = NULL;
const char DxEffectSkinMesh::NAME[] = "��Ų�޽� PLAY";

DxEffectSkinMesh::DxEffectSkinMesh(void):
				m_fElapsedTime(0.f),
				m_pd3dDevice(NULL),
				m_pSkinChar(NULL)
				, m_nConut(0)
{
	D3DXMatrixIdentity( &m_matWorld );
}

DxEffectSkinMesh::~DxEffectSkinMesh(void)
{
	SAFE_DELETE(m_pSkinChar);
	CleanUp ();
}

//Note: CreateDivece�� ReleaseDevice�� ��� DxEffSingleMan����
//      InvalidateDeviceObjects()�� RestoreDeviceObjects()���� ȣ���ϴ� �κ��̴�.
HRESULT DxEffectSkinMesh::CreateDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }
HRESULT DxEffectSkinMesh::ReleaseDevice( LPDIRECT3DDEVICEQ pd3dDevice )	{ return S_OK; }

void DxEffectSkinMesh::OnInitDevice_STATIC()
{
}

void DxEffectSkinMesh::OnDeleteDevice_STATIC()
{
}

void DxEffectSkinMesh::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
{
	D3DXVECTOR3 vMaxMesh(0.f,0.f,0.f);
	D3DXVECTOR3 vMinMesh(0.f,0.f,0.f);
	if ( m_pSkinChar )
	{
		vMaxMesh = m_pSkinChar->m_vMax;
		vMinMesh = m_pSkinChar->m_vMin;

		D3DXMatrixAABBTransform( vMaxMesh, vMinMesh, m_matWorld );

		if( vMax.x < vMaxMesh.x )	vMax.x = vMaxMesh.x;
		if( vMax.y < vMaxMesh.y )	vMax.y = vMaxMesh.y;
		if( vMax.z < vMaxMesh.z )	vMax.z = vMaxMesh.z;

		if( vMin.x > vMinMesh.x )	vMin.x = vMinMesh.x;
		if( vMin.y > vMinMesh.y )	vMin.y = vMinMesh.y;
		if( vMin.z > vMinMesh.z )	vMin.z = vMinMesh.z;
	}

	if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
	if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
}

HRESULT DxEffectSkinMesh::RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::RestoreDeviceObjects ( pd3dDevice );	

	if( m_pSkinChar ) m_pSkinChar->RestoreDeviceObjects( pd3dDevice );

	return S_OK;
}

HRESULT DxEffectSkinMesh::InvalidateDeviceObjects()
{	
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->InvalidateDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::DeleteDeviceObjects()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::DeleteDeviceObjects ();

	if( m_pSkinChar ) m_pSkinChar->DeleteDeviceObjects();

	return S_OK;
}

HRESULT DxEffectSkinMesh::FrameMove ( float fTime, float fElapsedTime )
{
	//	Note : Eff�� ���̸� ���.
	//
	m_fElapsedTime	= fElapsedTime; 
	m_fGAge			+= fElapsedTime;

	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	if ( m_dwRunFlag & EFF_PLY_END )		goto _RETURN;

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� z�ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( IsReadyToDie() )					m_dwRunFlag |= EFF_PLY_END;

	if ( m_pSkinChar )
	{
		m_pSkinChar->FrameMove ( fTime, fElapsedTime, FALSE, m_matWorld );
	}

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	DxEffSingle::FrameMove ( fTime, fElapsedTime );
	return S_OK;
}

HRESULT DxEffectSkinMesh::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	D3DXMatrixMultiply( &m_matWorld, &m_matLocal, &matComb );

	// CharAniThread �۾��� �ϸ鼭 �������� ������.
	// m_nConut>0 ���� ������ m_pSkinChar->FrameMove ( fTime, fElapsedTime, m_matWorld ); ���� m_matWorld ���� Identity �� ������ ���ܼ� �̷��� ��.
	if ( m_pSkinChar && (m_nConut>0) )
	{
		if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
		{
			m_pSkinChar->RenderFixed( pd3dDevice, TRUE, fAlpha, FALSE, TRUE, FALSE, FALSE, TRUE );
		}
		else
		{
			D3DXVECTOR3 vPosOffset(0.f,0.f,0.f);
			vPosOffset.x = m_matWorld._41 - m_pSkinChar->GetSkeletonRootMatrix()._41;
			vPosOffset.y = m_matWorld._42 - m_pSkinChar->GetSkeletonRootMatrix()._42;
			vPosOffset.z = m_matWorld._43 - m_pSkinChar->GetSkeletonRootMatrix()._43;

			// Ư���� �����.
			m_pSkinChar->SetPosOffset( vPosOffset );

			m_pSkinChar->SetPhysX_Cloth( TRUE );
			m_pSkinChar->RenderMaterialOpaque ( pd3dDevice, fAlpha );
			m_pSkinChar->RenderMaterialHardAlpha ( pd3dDevice, fAlpha );
			m_pSkinChar->RenderMaterialSoftAlpha ( pd3dDevice, fAlpha );
			m_pSkinChar->RenderMaterialEffect ( pd3dDevice, fAlpha );
		}
	}

	++m_nConut;

	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, m_matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	
	return S_OK;	
}


HRESULT DxEffectSkinMesh::FinalCleanup ()
{
	
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

//void DxEffectSkinMesh::SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert )
//{
//	if ( m_pSkinChar )
//	{
//		m_pSkinChar->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	}
//
//	if( m_pChild )		m_pChild->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//	if( m_pSibling )	m_pSibling->SetFlagWinnerClubCountry( stTextureName, vFlagConvert );
//}

void DxEffectSkinMesh::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( m_pSkinChar )
	{
		m_pSkinChar->SetWorldBattleFlag( pWorldBattleFlag );
	}

	if( m_pChild )		m_pChild->SetWorldBattleFlag( pWorldBattleFlag );
	if( m_pSibling )	m_pSibling->SetWorldBattleFlag( pWorldBattleFlag );
}