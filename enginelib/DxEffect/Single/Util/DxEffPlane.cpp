#include "pch.h"

#include "../../../DxTools/TextureManager.h"
#include "../../../DxTools/DxDynamicVB.h"

#include "../../../DxEffect/Frame/DxEffectMan.h"
#include "../../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../../SigmaCore/File/SerialFile.h"

#include "./DxEffPlane.h"

// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//---------------------------------------------------------------------[CAMERA_PROPERTY]
const DWORD	PLANE_PROPERTY::VERSION	= 0x0100;
const char	PLANE_PROPERTY::NAME[]	= "���";

DxEffSingle* PLANE_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
{
	DxEffPlane *pEffect = new DxEffPlane;

	//	Note : TransPorm Set.		[���뼳��]
	pEffect->m_matLocal = m_matLocal;

	// Note : �ð� ����.			[���뼳��]
	pEffect->m_fGBeginTime = m_fGBeginTime;
	pEffect->m_fGLifeTime  = m_fGLifeTime;

	// Note : ���ư��� ������Ʈ	[���뼳��]
	pEffect->m_bMoveObj	= m_bMoveObj;

	// Note : ���� ����
	pEffect->UpdateProperty( (*this) );

	return pEffect;
}

HRESULT PLANE_PROPERTY::SaveFile ( sc::SerialFile &SFile )
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

HRESULT PLANE_PROPERTY::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE )
{
	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	//	Note : ������ ��ġ�� ���. 
	//
	if( dwVer == VERSION )
	{
		//	Note : �θ� Ŭ������ ����.
		SFile.ReadBuffer ( m_matLocal, sizeof(D3DXMATRIX) );

		SFile >> m_bMoveObj;
		SFile >> m_fGBeginTime;
		SFile >> m_fGLifeTime;

		//	Note : ����Ʈ�� Property �� ����.
		//
		SFile.ReadBuffer ( &m_Property, sizeof(PROPERTY) );

		m_textureRes.Clear();
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
	hr = Create ( pd3dDevice, bBackUpCache );
	if ( FAILED(hr) )	return hr;

	//	Note : ����, �ڽ� �б�.
	//
	EFF_PROPERTY::LoadFile ( SFile, pd3dDevice, bBackUpCache, bOldDecalUSE );

	return S_OK;
}

HRESULT PLANE_PROPERTY::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
{
	m_textureRes = TextureManager::GetInstance().LoadTexture(
		m_szTexture,
		false,
		TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
		0,
		D3DFMT_UNKNOWN,
		TEXTURE_RESIZE_NONE,
		TEXTURE_EFFECT,
		true, 
		bBackUpCache );

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::InitDeviceObjects ( pd3dDevice, bBackUpCache );

	return S_OK;
}

HRESULT PLANE_PROPERTY::DeleteDeviceObjects ()
{
	m_textureRes.Clear();

	//	Note : ����, �ڽ�.
	//
	EFF_PROPERTY::DeleteDeviceObjects ();

	return S_OK;
}


//-------------------------------------------- [���� �ɹ� ���� �� �ʱ�ȭ] ----------------------------------------------
//
//
const DWORD	DxEffPlane::TYPEID	= EFFSINGLE_PLANE;
const DWORD	DxEffPlane::FLAG	= NULL;
const char	DxEffPlane::NAME[]	= "���";

LPDIRECT3DSTATEBLOCK9 DxEffPlane::m_pSavedRenderStats = NULL;
LPDIRECT3DSTATEBLOCK9 DxEffPlane::m_pDrawRenderStats  = NULL;

//	Note : 
//
BOOL DxEffPlane::VAILEDDEVICE = FALSE;

HRESULT DxEffPlane::CreateDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;	

	VAILEDDEVICE = TRUE;

	D3DCAPSQ d3dCaps;
	pd3dDevice->GetDeviceCaps ( &d3dCaps );

	for( UINT which=0; which<2; which++ )
	{
		pd3dDevice->BeginStateBlock();

		pd3dDevice->SetRenderState( D3DRS_COLORVERTEX,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_NONE );


		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x01 );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );

		if( which==0 )	pd3dDevice->EndStateBlock ( &m_pSavedRenderStats );
		else			pd3dDevice->EndStateBlock ( &m_pDrawRenderStats );
	}

	return S_OK;
}

HRESULT DxEffPlane::ReleaseDevice ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	VAILEDDEVICE = FALSE;

	SAFE_RELEASE( m_pSavedRenderStats );
	SAFE_RELEASE( m_pDrawRenderStats );

	return S_OK;
}

void DxEffPlane::OnInitDevice_STATIC()
{
}

void DxEffPlane::OnDeleteDevice_STATIC()
{
}


HRESULT DxEffPlane::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

	//	Note : ����, �ڽ� ȣ��
	//
	if ( m_rThisGroup )
	{
		DxEffSingle::RestoreDeviceObjects ( pd3dDevice );
	}
	else
	{
		if ( m_pChild )		m_pChild->RestoreDeviceObjects ( pd3dDevice );
		if ( m_pSibling )	m_pSibling->RestoreDeviceObjects ( pd3dDevice );
	}

	return S_OK;
}

HRESULT DxEffPlane::InvalidateDeviceObjects ()
{
	//	Note : ����, �ڽ� ȣ��
	//
	DxEffSingle::InvalidateDeviceObjects ();

	return S_OK;
}

//-------------------------------------------- [Ŭ���� ��ü] ------------------------------------------------------------
//
//
DxEffPlane::DxEffPlane () 
: m_pd3dDevice(NULL)
, m_pVB(NULL)
, m_dwVerSize(50)
{
}

DxEffPlane::~DxEffPlane ()
{
	SAFE_RELEASE(m_pVB);
	CleanUp ();
}

HRESULT DxEffPlane::FinalCleanup()
{
	//	Note : ���� �ڽ� ���.
	//
	DxEffSingle::FinalCleanup ();

	return S_OK;
}

HRESULT DxEffPlane::FrameMove ( float fTime, float fElapsedTime )
{
	D3DXMATRIX	matLocal;

	if ( m_dwRunFlag & EFF_PLY_END )
	{
		goto _RETURN;
	}

	//	Note : Eff�� ���̸� ���.
	//
	DxEffSingle::m_fGAge += fElapsedTime;


	//	Note : �ݺ��� �� �ð� �� ���� ���� �ʱ�ȭ �Ѵ�.
	//
	if ( m_rThisGroup && m_rThisGroup->m_dwFlag&EFF_CFG_NEVERDIE )
	{
		if ( m_fGAge>=(m_rThisGroup->m_fGBeginTime+m_rThisGroup->m_fGLifeTime) )
		{
			m_fGAge		= 0.f;
			m_dwRunFlag &= ~EFF_PLY_PLAY;
			m_dwRunFlag &= ~EFF_PLY_DONE;
			m_dwRunFlag &= ~EFF_PLY_END;
		}
	}

	//	Note : ���۵��� �ʾҴ��� �˻�, ���� �ð��̸� Ȱ��ȭ ��Ŵ.
	//
	if ( !( DxEffSingle::m_dwRunFlag & EFF_PLY_PLAY ) )
	{
		if ( m_fGAge >= m_fGBeginTime )		m_dwRunFlag |= EFF_PLY_PLAY;
		else								goto _RETURN;
	}

	//	Note : ���� ������ �Ǿ����� �˻�.
	//
	if ( m_rThisGroup )
	{
		if ( IsReadyToDie() )					
			DxEffSingle::m_dwRunFlag |= EFF_PLY_END;
	}

	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_RENDPASS )		
		goto _RETURN;	// ��鸮�� �ʵ���

	//float		AddY;
	//D3DXVECTOR3	*pFromPt = &DxViewPort::GetInstance().GetFromPt();
	//D3DXVECTOR3	*pLookatPt = &DxViewPort::GetInstance().GetLookatPt();

	//switch ( m_iCameraSet )
	//{
	//	//	ī�޶� �� �Ʒ��� ����
	//case 1:
	//	if ( m_fTimeSum > m_fPlayTime )
	//	{
	//		m_fTimeSum = 0.f;
	//		AddY = 1.f*m_fScale*((rand()%10)/10.f);

	//		if ( m_vFromPtSum.y > 0.0f )
	//		{
	//			m_vFromPtSum.y		-= AddY;
	//			m_vLookatPtSum.y	-= AddY;
	//			pFromPt->y			-= AddY;
	//			pLookatPt->y		-= AddY;
	//		}
	//		else
	//		{
	//			m_vFromPtSum.y		+= AddY;
	//			m_vLookatPtSum.y	+= AddY;			
	//			pFromPt->y			+= AddY;
	//			pLookatPt->y		+= AddY;
	//		}
	//	}
	//	break;
	//	//	Zoom In/Out
	//case 2:
	//	AddY = 1.f*m_fScale*((rand()%10)/10.f);

	//	if ( m_vFromPtSum.y > 0.0f )
	//	{
	//		m_vFromPtSum.y		-= AddY;
	//		m_vLookatPtSum.y	-= AddY;
	//		pFromPt->y			-= AddY;
	//		pLookatPt->y		-= AddY;
	//	}
	//	else
	//	{
	//		m_vFromPtSum.y		+= AddY;
	//		m_vLookatPtSum.y	+= AddY;			
	//		pFromPt->y			+= AddY;
	//		pLookatPt->y		+= AddY;
	//	}
	//	break;
	//}

	UpdateProperty();

_RETURN:
	//	Note : ����, �ڽ� ���.
	//

	if ( m_rThisGroup )
	{
		DxEffSingle::FrameMove( fTime, fElapsedTime );
		DxEffSingle::FrameMove( fTime, fElapsedTime );
	}
	else
	{
		if ( m_pChild )		m_pChild->FrameMove( fTime, fElapsedTime );
		if ( m_pSibling )	m_pSibling->FrameMove ( fTime, fElapsedTime );
	}

	return S_OK;
}

HRESULT DxEffPlane::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	if ( !std::string(m_Property.m_szTexture).empty() && !m_Property.m_textureRes.GetTexture() )
	{
		return S_FALSE;
	}

	D3DXMATRIX matWorld = m_matLocal;
	D3DXMatrixMultiply ( &matWorld, &m_matLocal, &matComb );

	if ( !(DxEffSingle::m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( DxEffSingle::m_dwRunFlag&EFF_PLY_RENDPASS )	goto _RETURN;

	if ( !m_pVB )
	{
		goto _RETURN;
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();

	{
		if( m_Property.m_textureRes.Update() )
		{
			pd3dDevice->SetTexture(0, m_Property.m_textureRes.GetTexture());
		}
		else
		{
			pd3dDevice->SetTexture(0, NULL);
		}


		pd3dDevice->SetIndices( NULL );
		pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof(VERTEXCOLORTEX) );
		pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );

		pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, CalculateFaceNum() );
	}

	m_pSavedRenderStats->Apply();

_RETURN:
	//	Note : ����, �ڽ� ���.
	//
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matWorld, fAlpha );

	return hr;
}

void DxEffPlane::UpdateProperty( const PLANE_PROPERTY& cProperty )
{
	bool bRebuild   = false;
	bool bTexReload = false;

	if ( m_Property.ISREBUILD(cProperty) )
	{
		bRebuild = true;
	}

	std::string strTextureSrc(cProperty.m_szTexture);
	std::string strTextureDst(m_Property.m_szTexture);

	if ( strTextureDst != strTextureSrc )
	{
		bTexReload = true;
	}

	//! �ؽ��� ������ �������� �ʴ´�.
	TextureResource OrgResource = m_Property.m_textureRes;
	m_Property = cProperty;
	m_Property.m_textureRes = OrgResource;

	UpdateProperty( bTexReload, bRebuild );
}

void DxEffPlane::UpdateProperty( const bool bTexReload, const bool bRebuild )
{
	if ( !m_pd3dDevice )
	{
		return;
	}

	//! �ؽ��� �ε�
	if ( bTexReload || !m_Property.m_textureRes.IsValid() || !m_Property.m_textureRes.GetTexture() )
	{
		m_Property.m_textureRes.Clear();
		std::string strTexture = m_Property.m_szTexture;

		if ( !strTexture.empty() )
		{
			m_Property.m_textureRes = TextureManager::GetInstance().LoadTexture(
				m_Property.m_szTexture,
				false,
				TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
				0,
				D3DFMT_UNKNOWN,
				TEXTURE_RESIZE_NONE,
				TEXTURE_EFFECT,
				true,
				FALSE );
		}
	}

	DWORD dwVerCount = CalculateVerticesNum();

	if ( m_dwVerSize < dwVerCount )
	{
		m_dwVerSize = dwVerCount;
		SAFE_RELEASE(m_pVB);
	}

	if ( !m_pVB )
	{
		HRESULT hr = m_pd3dDevice->CreateVertexBuffer(
			m_dwVerSize*sizeof(VERTEXCOLORTEX),
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
			VERTEXCOLORTEX::FVF,
			D3DPOOL_SYSTEMMEM,
			&m_pVB,
			NULL );

		BuildPlane();
	}
	else if ( bRebuild )
	{
		BuildPlane();
	}
}

const DWORD DxEffPlane::CalculateFaceNum()
{
	return 2;
}

const DWORD DxEffPlane::CalculateVerticesNum()
{
	return 6;
}

void DxEffPlane::BuildPlane()
{
	if ( !m_pVB )
	{
		return;
	}

	VERTEXCOLORTEX* pLocked = NULL;
	HRESULT hResult = m_pVB->Lock( 0, 0, (VOID**)&pLocked, 0 );

	if ( SUCCEEDED(hResult) )
	{
		const D3DXVECTOR3 vStandardNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 vNormal = m_Property.m_vNormal;

		if ( D3DXVec3LengthSq(&vNormal) <= D3DX_16F_EPSILON )
		{
			vNormal = vStandardNormal;
		}

		D3DXVec3Normalize(&vNormal, &vNormal);

		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vStandardNormal, &vNormal );

		D3DXVec3Normalize(&vAxis, &vAxis);

		float fRotation = acos( D3DXVec3Dot(&vStandardNormal, &vNormal) );

		D3DXMATRIX matRotation;
		D3DXMatrixRotationAxis( &matRotation, &vAxis, fRotation );

		const float fHalfWidth  = m_Property.m_fWidth*0.5f;
		const float fHalfHeight = m_Property.m_fHeight*0.5f;

		{
			pLocked->vPos	 = D3DXVECTOR3(-fHalfWidth, 0.0f, -fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;

			pLocked->vPos	 = D3DXVECTOR3(fHalfWidth, 0.0f, -fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 1.0f, 0.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;

			pLocked->vPos	 = D3DXVECTOR3(fHalfWidth, 0.0f, fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;
			
			pLocked->vPos	 = D3DXVECTOR3(fHalfWidth, 0.0f, fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 1.0f, 1.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;

			pLocked->vPos	 = D3DXVECTOR3(-fHalfWidth, 0.0f, fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 0.0f, 1.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;

			pLocked->vPos	 = D3DXVECTOR3(-fHalfWidth, 0.0f, -fHalfHeight);
			pLocked->dwColor = m_Property.m_dwColor;
			pLocked->vTex	 = D3DXVECTOR2( 0.0f, 0.0f );
			D3DXVec3TransformCoord( &pLocked->vPos, &pLocked->vPos, &matRotation );
			++pLocked;
		}
	}	
	m_pVB->Unlock();
}